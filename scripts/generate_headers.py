#!/usr/bin/env python3
"""
scripts/generate_headers.py
Header generator for IFJ26 compiler source files. It scans the src/ directory for .c and .h files,
and generates standardized header comments with project info, file name, brief description,
and author information. It can also update existing headers while preserving authors and brief descriptions.

Usage:
    python3 scripts/generate_headers.py [--force|-f] [files...]

AI has been used to generate the initial version of this script, but it has been reviewed and modified by Aleš Obr (xobrale00) to ensure accuracy and adherence to project standards.
Prompt: "Write a Python 3 script to parse and update C/H Doxygen headers, 
accumulate authors based on git config/team.json, and enforce canonical include guards." - and several iteration while debugging this script
Founded (and solved) bugs:
- The script was unable to handle multiple authors
- The script was saying, that anyone who opens the file is an author, even if they didn't commit anything
- The script was not able to handle include guards properly, and it was not able to handle files without any authors at all
- The script has tendancy to insert headers several times in single file

"""

import json
from pathlib import Path
import re
import subprocess
import sys

CLR_RESET = "\033[0m"
CLR_GREEN = "\033[38;5;46m"
CLR_YELLOW = "\033[38;5;220m"
CLR_CYAN = "\033[38;5;51m"
CLR_RED = "\033[38;5;196m"

VALID_EXTS = {".c", ".h"}

HEADER_COMMENT_REGEX = re.compile(r"^\s*/\*\*.*?\*/\s*", re.DOTALL)

GUARD_START_RE = re.compile(
    r"^\s*#ifndef\s+([a-zA-Z0-9_]+)\s*\n\s*#define\s+\1[^\n]*\n?",
    re.MULTILINE,
)

GUARD_END_RE = re.compile(r"\n\s*#endif(?:\s*/\*.*?\*/|\s*//[^\n]*)?\s*$")


def load_team_config(root: Path) -> dict[str, str]:
    """Vrátí slovník mapující jakoukoliv identitu (jméno, email, login)
    na kanonický řetězec: 'Jméno Příjmení (xlogin)'
    """
    config_file = root / "team.json"
    lookup = {}

    if not config_file.exists():
        print(
            f"{CLR_YELLOW}[VAROVÁNÍ]{CLR_RESET} Soubor team.json nenalezen v"
            " kořeni projektu. Používám fallback z gitu."
        )
        return lookup

    try:
        data = json.loads(config_file.read_text(encoding="utf-8"))
        for member in data.get("members", []):
            name = member["name"]
            login = member["login"].lower().strip()
            canonical = f"{name} ({login})"

            lookup[login] = canonical
            lookup[name.lower()] = canonical

            for alias in member.get("identities", []):
                lookup[alias.lower().strip()] = canonical
    except Exception as e:
        print(
            f"{CLR_RED}[CHYBA]{CLR_RESET} Neplatný formát team.json:"
            f" {e}\nPokračuji s fallbackem."
        )

    return lookup


def resolve_author_string(raw_identifier: str, team_lookup: dict[str, str]) -> str:
    """Převede surový řetězec autora na kanonický formát pomocí team.json."""
    clean_id = raw_identifier.strip()
    lower_id = clean_id.lower()

    if lower_id in team_lookup:
        return team_lookup[lower_id]

    login_match = re.search(r"\b(x[a-z]{5}\d{2})\b", lower_id)
    if login_match and login_match.group(1) in team_lookup:
        return team_lookup[login_match.group(1)]

    for key, canonical in team_lookup.items():
        if key in lower_id:
            return canonical

    return clean_id


def get_current_git_author(team_lookup: dict[str, str]) -> str:
    """Vrátí kanonickou identitu aktuálního lokálního uživatele gitu."""
    try:
        name = subprocess.check_output(
            ["git", "config", "user.name"], text=True, stderr=subprocess.DEVNULL
        ).strip()
        email = subprocess.check_output(
            ["git", "config", "user.email"], text=True, stderr=subprocess.DEVNULL
        ).strip()
    except Exception:
        return "IFJ26 Team Developer (xlogin00)"

    for probe in [name, email, email.split("@")[0]]:
        if probe.lower() in team_lookup:
            return team_lookup[probe.lower()]

    login = email.split("@")[0] if "@" in email else "xlogin00"
    return f"{name} ({login})"


def get_file_git_authors(filepath: Path, team_lookup: dict[str, str]) -> list[str]:
    """Získá seznam autorů, kteří prokazatelně commitovali do daného souboru."""
    try:
        out = subprocess.check_output(
            ["git", "log", "--follow", "--format=%an <%ae>", str(filepath)],
            text=True,
            stderr=subprocess.DEVNULL,
        ).strip()
        if not out:
            return []

        authors = []
        for line in out.splitlines():
            res = resolve_author_string(line, team_lookup)
            if res not in authors:
                authors.append(res)
        return authors
    except Exception:
        return []


def extract_existing_brief(header_text: str) -> str | None:
    match = re.search(r"@brief\s+(.+)$", header_text, re.MULTILINE)
    if match:
        return match.group(1).strip()
    return None


def derive_brief(filepath: Path) -> str:
    stem = filepath.stem.lower()
    is_header = filepath.suffix == ".h"
    role = "interface" if is_header else "implementation"

    descriptions = {
        "scanner": f"Lexical analyzer {role} for IFJ26 language compiler.",
        "parser": f"Syntactic analyzer and recursive descent {role}.",
        "psa": f"Operator-precedence expression parser {role}.",
        "precedence": f"Operator-precedence expression parser {role}.",
        "symtable": f"Symbol table (TRP/AVL) {role}.",
        "codegen": (
            f"Target code generator {role} emitting IFJcode26 instructions."
        ),
        "ast": f"Abstract syntax tree data structures and {role}.",
        "str": f"Dynamic string buffer utility {role}.",
        "arena": f"Region-based memory arena allocator {role}.",
        "error": f"Error codes and diagnostic reporter {role}.",
        "main": "Compiler entry point and pipeline orchestration.",
    }
    return descriptions.get(
        stem,
        f"{filepath.stem.replace('_', ' ').capitalize()} module {role} for IFJ26"
        " compiler.",
    )


def extract_existing_authors(
    header_text: str, team_lookup: dict[str, str]
) -> list[str]:
    authors = []
    for line in header_text.splitlines():
        match = re.search(r"@author\s+(.+)$", line)
        if match:
            canonical = resolve_author_string(match.group(1), team_lookup)
            if canonical not in authors:
                authors.append(canonical)
    return authors


def generate_header(
    filepath: Path, authors: list[str], custom_brief: str | None = None
) -> str:
    brief = custom_brief if custom_brief else derive_brief(filepath)
    author_lines = "\n".join([f" * @author {a}" for a in authors])
    return (
        "/**\n"
        " * @project IFJ26 Compiler\n"
        f" * @file {filepath.name}\n"
        f" * @brief {brief}\n"
        f"{author_lines}\n"
        " * @note Standard: ISO C11\n"
        " */\n\n"
    )


def strip_all_outer_guards(code: str) -> str:
    current = code.strip()
    while True:
        start_match = GUARD_START_RE.match(current)
        end_match = GUARD_END_RE.search(current)

        if start_match and end_match and start_match.end() < end_match.start():
            current = current[start_match.end() : end_match.start()].strip()
        else:
            break
    return current


def clean_and_wrap_header_body(body: str, guard_name: str) -> str:
    inner_code = strip_all_outer_guards(body)
    return (
        f"#ifndef {guard_name}\n"
        f"#define {guard_name}\n\n"
        f"{inner_code}\n\n"
        f"#endif /* {guard_name} */\n"
    )


def process_file(
    filepath: Path,
    current_author: str,
    team_lookup: dict[str, str],
    force: bool = False,
) -> bool:
    raw_content = filepath.read_text(encoding="utf-8")

    match = HEADER_COMMENT_REGEX.search(raw_content)
    existing_brief = None
    existing_authors: list[str] = []

    if match and match.start() < 200:
        existing_header = match.group(0)
        body = (
            raw_content[: match.start()] + raw_content[match.end() :]
        ).strip()

        existing_brief = extract_existing_brief(existing_header)
        existing_authors = extract_existing_authors(existing_header, team_lookup)

        is_valid = (
            f"@file {filepath.name}" in existing_header
            and "@project IFJ26 Compiler" in existing_header
            and "@author" in existing_header
            and "@brief" in existing_header
            and "@note Standard: ISO C11" in existing_header
        )

        # Pokud je hlavička validní a nevynucujeme rebuild, prověříme, zda je třeba sahat do souboru
        if is_valid and not force:
            # Zjistíme, jestli se v gitu objevil nový autor, který ještě není v hlavičce
            git_authors = get_file_git_authors(filepath, team_lookup)
            has_new_git_author = any(ga not in existing_authors for ga in git_authors)

            if not has_new_git_author:
                if filepath.suffix != ".h":
                    return False
                
                # U .h souborů ještě zkontrolujeme include guardy
                guard_name = f"IFJ26_{filepath.stem.upper()}_H"
                stripped_body = body.strip()
                start_match = GUARD_START_RE.match(stripped_body)
                end_match = GUARD_END_RE.search(stripped_body)
                if start_match and end_match and start_match.group(1) == guard_name:
                    inner = stripped_body[
                        start_match.end() : end_match.start()
                    ].strip()
                    if not GUARD_START_RE.search(inner) and not GUARD_END_RE.search(inner):
                        return False
    else:
        body = raw_content.strip()

    # Sestavení finálního seznamu autorů
    authors: list[str] = list(existing_authors)

    # Přidáme autory z commit logu daného souboru
    git_authors = get_file_git_authors(filepath, team_lookup)
    for ga in git_authors:
        if ga not in authors:
            authors.append(ga)

    # Fallback: pokud soubor nemá žádného autora (ani v hlavičce, ani v gitu),
    # dostane ho ten, kdo soubor zrovna vytvořil
    if not authors:
        authors = [current_author]

    new_header = generate_header(filepath, authors, custom_brief=existing_brief)

    if filepath.suffix == ".h":
        guard_name = f"IFJ26_{filepath.stem.upper()}_H"
        wrapped_body = clean_and_wrap_header_body(body, guard_name)
        final_content = f"{new_header}{wrapped_body}"
    else:
        final_content = f"{new_header}{body}\n"

    if final_content != raw_content:
        filepath.write_text(final_content, encoding="utf-8")
        return True

    return False


def main():
    root = Path(__file__).resolve().parent.parent
    src_dir = root / "src"

    if not src_dir.exists():
        print(f"{CLR_RED}[CHYBA]{CLR_RESET} Adresář {src_dir} nenalezen.")
        sys.exit(1)

    team_lookup = load_team_config(root)
    force = "--force" in sys.argv or "-f" in sys.argv
    raw_args = [a for a in sys.argv[1:] if a not in ("--force", "-f")]

    current_author = get_current_git_author(team_lookup)

    if raw_args:
        files = [
            Path(arg).resolve()
            for arg in raw_args
            if Path(arg).suffix in VALID_EXTS
        ]
    else:
        files = sorted([p for p in src_dir.rglob("*") if p.suffix in VALID_EXTS])

    modified = 0
    for f in files:
        if process_file(f, current_author, team_lookup, force=force):
            print(
                f"  {CLR_GREEN}✔{CLR_RESET} Aktualizována hlavička ->"
                f" {CLR_CYAN}{f.relative_to(root)}{CLR_RESET}"
            )
            modified += 1

    if modified > 0:
        print(f"\n{CLR_GREEN}[HOTOVO]{CLR_RESET} Upraveno {modified} souborů.")
    else:
        print(
            f"{CLR_GREEN}[OK]{CLR_RESET} Všechny soubory jsou 100% synchronní se"
            " standardem."
        )


if __name__ == "__main__":
    main()