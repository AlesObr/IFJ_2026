#!/usr/bin/env python3
"""
scripts/lint_comments.py
Kontrola sterilní čistoty kódu v projektu IFJ26:
- Rekurzivní průchod větveným stromem src/
- Striktní angličtina (zákaz diakritiky vyjma tagu @author v hlavičce)
- Povinné C11 Doxygen hlavičky pro .c i .h
- V .h souborech povinný Doxygen blok (/** ... */) nad každou deklarací funkce
- Zákaz kacířských slov (TODO, Křivka, Ghidra, AI zmínky...)
"""

import sys
import re
from pathlib import Path

# ANSI barvy a typografie
CLR_RESET   = "\033[0m"
CLR_BOLD    = "\033[1m"
CLR_DIM     = "\033[2m"
CLR_RED     = "\033[38;5;196m"
CLR_GREEN   = "\033[38;5;46m"
CLR_YELLOW  = "\033[38;5;220m"
CLR_BLUE    = "\033[38;5;39m"
CLR_MAGENTA = "\033[38;5;201m"
CLR_CYAN    = "\033[38;5;51m"
CLR_GRAY    = "\033[38;5;245m"

VALID_EXTS = {".c", ".h"}

# Zakázané řetězce kdekoliv v komentářích
BANNED_REGEX = re.compile(
    r"\b(TODO|FIXME|HACK|GHIDRA|IC26INT|KRIVKA|KŘIVKA|COPILOT|CHATGPT|LLM|PROMPT|DO_PRDELE|KURV|HOVNO)\b",
    re.IGNORECASE
)

# Záchyt české a slovenské diakritiky
CZECH_CHARS_RE = re.compile(r"[áčďéěíňóřšťúůýžÁČĎÉĚÍŇÓŘŠŤÚŮÝŽ]")

# Detekce deklarace funkce v .h souboru
FUNC_DECL_REGEX = re.compile(r"^\s*([a-zA-Z_][a-zA-Z0-9_* ]+)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\([^)]*\)\s*;")

def format_loc(filepath: Path, line: int, col: int) -> str:
    """Zformátuje cestu na standardní souřadnice pro terminál/IDE s barvami."""
    return f"{CLR_CYAN}{filepath}{CLR_RESET}:{CLR_YELLOW}{line}{CLR_RESET}:{CLR_MAGENTA}{col}{CLR_RESET}"

def find_project_root() -> Path:
    """Najde kořen repozitáře (hledá .git směrem nahoru, jinak vezme parent skriptu)."""
    current = Path(__file__).resolve().parent
    while current != current.parent:
        if (current / ".git").exists() or (current / "Makefile").exists():
            return current
        current = current.parent
    return Path(__file__).resolve().parent.parent

def check_file_doxygen_header(filepath: Path, lines: list[str]) -> list[str]:
    """Zkontroluje formální Doxygen hlavičku na prvních řádcích souboru (.c i .h)."""
    errors = []
    if not lines or not lines[0].strip().startswith("/**"):
        loc = format_loc(filepath, 1, 1)
        errors.append(f"{loc} {CLR_RED}[HLAVIČKA]{CLR_RESET} Chybí zahájení Doxygen bloku '/**' na řádku 1.")
        return errors

    header_block = "\n".join(lines[:15])

    # 1. Musí obsahovat přesné jméno souboru
    if f"@file {filepath.name}" not in header_block:
        loc = format_loc(filepath, 2, 1)
        errors.append(f"{loc} {CLR_RED}[HLAVIČKA]{CLR_RESET} V hlavičce chybí přesný název: '{CLR_BOLD}@file {filepath.name}{CLR_RESET}'")

    # 2. Povinné formální náležitosti
    for tag in ["@brief", "@author"]:
        if tag not in header_block:
            loc = format_loc(filepath, 1, 1)
            errors.append(f"{loc} {CLR_RED}[HLAVIČKA]{CLR_RESET} V hlavičce chybí povinná položka: '{CLR_YELLOW}{tag}{CLR_RESET}'")

    return errors

def check_header_functions_documented(filepath: Path, lines: list[str]) -> list[str]:
    """
    Vynutí, aby každá funkce v .h souboru měla bezprostředně nad sebou
    Doxygen komentář (/** ... */).
    """
    errors = []

    for idx, line in enumerate(lines, start=1):
        stripped = line.strip()

        if stripped.startswith("typedef") or stripped.startswith("#") or not stripped:
            continue

        match = FUNC_DECL_REGEX.match(line)
        if match:
            func_name = match.group(2)
            
            prev_idx = idx - 2
            while prev_idx >= 0 and not lines[prev_idx].strip():
                prev_idx -= 1

            has_doxygen = False
            if prev_idx >= 0:
                if lines[prev_idx].strip().endswith("*/"):
                    block_walk = prev_idx
                    while block_walk >= 0:
                        line_text = lines[block_walk].strip()
                        if "/**" in line_text:
                            has_doxygen = True
                            break
                        if "/*" in line_text:
                            break
                        block_walk -= 1

            if not has_doxygen:
                loc = format_loc(filepath, idx, 1)
                errors.append(
                    f"{loc} {CLR_YELLOW}[DOXYGEN]{CLR_RESET} Funkce '{CLR_BOLD}{func_name}(){CLR_RESET}' nemá nad sebou předepsaný Doxygen blok (/** ... */)!"
                )

    return errors

def extract_comments_and_check(filepath: Path, content: str) -> list[str]:
    """Vyparsuje komentáře s přesnými čísly řádků a provede jazykový audit."""
    errors = []
    lines = content.splitlines()
    in_block = False
    in_file_header = False

    for idx, line in enumerate(lines, start=1):
        stripped = line.strip()

        # Detekce úvodní hlavičky souboru na řádku 1
        if idx == 1 and stripped.startswith("/**"):
            in_file_header = True

        if in_block:
            if "*/" in line:
                in_block = False
                if in_file_header:
                    in_file_header = False
            comment_to_check = line
        elif "/*" in line:
            in_block = True
            comment_to_check = line.split("/*", 1)[1]
            if "*/" in line:
                in_block = False
                if in_file_header:
                    in_file_header = False
        elif "//" in line:
            comment_to_check = line.split("//", 1)[1]
        else:
            continue

        loc = format_loc(filepath, idx, 1)

        # Výjimka: Povolit diakritiku v hlavičce POUZE na řádku s tagem @author
        if in_file_header and "@author" in comment_to_check:
            # Zkontrolujeme pouze zakázaná slova (žádný TODO/Křivka ani v autorovi)
            match = BANNED_REGEX.search(comment_to_check)
            if match:
                banned_word = match.group(0)
                errors.append(
                    f"{loc} {CLR_RED}{CLR_BOLD}[BEZPEČNOST]{CLR_RESET} Zakázaný výraz '{CLR_RED}{banned_word}{CLR_RESET}' v komentáři!"
                )
            continue

        # 1. Kontrola češtiny/slovenštiny (všude jinde)
        if CZECH_CHARS_RE.search(comment_to_check):
            snippet = comment_to_check.strip()
            if len(snippet) > 45:
                snippet = snippet[:42] + "..."
            errors.append(
                f"{loc} {CLR_YELLOW}[JAZYK]{CLR_RESET} Detekována CZ/SK diakritika: \"{CLR_GRAY}{snippet}{CLR_RESET}\""
            )

        # 2. Kontrola zakázaných slov
        match = BANNED_REGEX.search(comment_to_check)
        if match:
            banned_word = match.group(0)
            errors.append(
                f"{loc} {CLR_RED}{CLR_BOLD}[BEZPEČNOST]{CLR_RESET} Zakázaný výraz '{CLR_RED}{banned_word}{CLR_RESET}' v komentáři!"
            )

    return errors

def lint_file(filepath: Path) -> list[str]:
    try:
        content = filepath.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        loc = format_loc(filepath, 1, 1)
        return [f"{loc} {CLR_RED}[ENCODING]{CLR_RESET} Soubor není v platném UTF-8 kódování!"]

    lines = content.splitlines()
    errors = []
    
    # 1. Společná hlavička souboru na řádku 1
    errors.extend(check_file_doxygen_header(filepath, lines))
    
    # 2. Pokud je to .h soubor, každá deklarovaná funkce musí mít /** ... */ blok
    if filepath.suffix == ".h":
        errors.extend(check_header_functions_documented(filepath, lines))

    # 3. Kontrola sterility textu všech komentářů
    errors.extend(extract_comments_and_check(filepath, content))
    
    return errors

def main():
    root = find_project_root()
    src_dir = root / "src"

    if not src_dir.exists():
        print(f"\n{CLR_RED}{CLR_BOLD}[FATAL]{CLR_RESET} Adresář 'src' nenalezen v {CLR_CYAN}{root}{CLR_RESET}\n")
        sys.exit(2)

    if len(sys.argv) > 1:
        raw_files = [Path(arg).resolve() for arg in sys.argv[1:]]
        files_to_check = [
            f for f in raw_files
            if f.suffix in VALID_EXTS and f.exists() and src_dir in f.parents
        ]
    else:
        files_to_check = sorted([
            p for p in src_dir.rglob("*")
            if p.is_file() and p.suffix in VALID_EXTS
        ])

    total_errors = []
    for filepath in files_to_check:
        total_errors.extend(lint_file(filepath))

    if total_errors:
        print(f"\n{CLR_RED}{CLR_BOLD}╔{'═' * 70}╗{CLR_RESET}")
        print(f"{CLR_RED}{CLR_BOLD}║  [INKVIZICE] KONTROLA KOMENTÁŘŮ A HLAVIČEK SELHALA ({len(total_errors):02d} chyb)     ║{CLR_RESET}")
        print(f"{CLR_RED}{CLR_BOLD}╚{'═' * 70}╝{CLR_RESET}\n")
        
        for err in total_errors:
            print(f"  {CLR_RED}✖{CLR_RESET} {err}")
            
        print(f"\n{CLR_YELLOW}{CLR_BOLD}Pravidla pro sterilní C11 kód:{CLR_RESET}")
        print(f"  {CLR_BLUE}1.{CLR_RESET} Výhradně technická angličtina (diakritika povolena POUZE v @author).")
        print(f"  {CLR_BLUE}2.{CLR_RESET} Nulová tolerance pro TODO, FIXME, Křivku, Ghidru, Copilota.")
        print(f"  {CLR_BLUE}3.{CLR_RESET} Povinná Doxygen hlavička na řádku 1 s @file a ISO C11.")
        print(f"  {CLR_BLUE}4.{CLR_RESET} V .h souborech musí být nad KAŽDOU funkcí blok /** ... */.\n")
        sys.exit(1)

    print(f"{CLR_GREEN}{CLR_BOLD}[OK]{CLR_RESET} Zkontrolováno {CLR_CYAN}{len(files_to_check)}{CLR_RESET} souborů (.c/.h) v {CLR_DIM}src/**{CLR_RESET} — {CLR_GREEN}vše je sterilní.{CLR_RESET}")
    sys.exit(0)

if __name__ == "__main__":
    main()