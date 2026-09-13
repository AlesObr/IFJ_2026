#!/usr/bin/env python3
"""
scripts/generate_headers.py
Automatické generování a doplňování sterilních Doxygen hlaviček pro IFJ26:
- Získá identitu autora z git configu (nebo fallback).
- Inteligentně odvodí popis modulu z názvu souboru.
- Vloží Doxygen blok na začátek souboru, pokud chybí.
"""

import sys
import subprocess
from pathlib import Path

CLR_RESET = "\033[0m"
CLR_GREEN = "\033[38;5;46m"
CLR_YELLOW = "\033[38;5;220m"
CLR_CYAN = "\033[38;5;51m"

VALID_EXTS = {".c", ".h"}

def get_git_author() -> str:
    """Získá jméno a login/email autora z git konfigurace."""
    try:
        name = subprocess.check_output(["git", "config", "user.name"], text=True).strip()
        email = subprocess.check_output(["git", "config", "user.email"], text=True).strip()
        login = email.split("@")[0] if "@" in email else email
        return f"{name} ({login})"
    except Exception:
        return "IFJ26 Team Developer"

def derive_brief(filepath: Path) -> str:
    """Odvodí stručný anglický popis podle názvu souboru a kontextu."""
    stem = filepath.stem.lower()
    is_header = filepath.suffix == ".h"
    role = "interface" if is_header else "implementation"

    descriptions = {
        "scanner": f"Lexical analyzer {role} for IFJ26 language compiler.",
        "parser": f"Syntactic analyzer and recursive descent {role}.",
        "precedence": f"Operator-precedence expression parser {role}.",
        "symtable": f"Symbol table (hash map) {role}.",
        "generator": f"Target code generator {role} emitting IFJcode26 instructions.",
        "ast": f"Abstract syntax tree node structures and {role}.",
        "str": f"Dynamic string buffer utility {role}.",
        "arena": f"Region-based memory arena allocator {role}.",
        "error": f"Error codes and diagnostic reporter {role}.",
        "main": "Compiler entry point and pipeline orchestration.",
    }

    return descriptions.get(stem, f"{filepath.stem.replace('_', ' ').capitalize()} module {role} for IFJ26 compiler.")

def generate_header(filepath: Path, author: str) -> str:
    brief = derive_brief(filepath)
    return (
        "/**\n"
        f" * @file {filepath.name}\n"
        f" * @brief {brief}\n"
        f" * @author {author}\n"
        " * @note Standard: ISO C11\n"
        " */\n\n"
    )

def ensure_header(filepath: Path, author: str) -> bool:
    content = filepath.read_text(encoding="utf-8")
    lines = content.splitlines()

    # Zjistíme, zda už platná Doxygen hlavička existuje
    if lines and lines[0].strip().startswith("/**"):
        header_peek = "\n".join(lines[:15])
        if f"@file {filepath.name}" in header_peek and "@note Standard: ISO C11" in header_peek:
            return False  # Hlavička už je v pořádku

    # Pokud tam visí neúplná stará hlavička, nebudeme ji duplikovat
    # Vložíme novou hlavičku před původní kód
    new_header = generate_header(filepath, author)
    filepath.write_text(new_header + content, encoding="utf-8")
    return True

def main():
    root = Path(__file__).resolve().parent.parent
    src_dir = root / "../src"

    if not src_dir.exists():
        print(f"Adresář {src_dir} nenalezen.")
        sys.exit(1)

    author = get_git_author()
    
    # Seznam souborů ze vstupu nebo rekurzivní sweep
    if len(sys.argv) > 1:
        files = [Path(arg).resolve() for arg in sys.argv[1:] if Path(arg).suffix in VALID_EXTS]
    else:
        files = sorted([p for p in src_dir.rglob("*") if p.suffix in VALID_EXTS])

    modified = 0
    for f in files:
        if ensure_header(f, author):
            print(f"  {CLR_GREEN}✔{CLR_RESET} Doplněna hlavička -> {CLR_CYAN}{f.relative_to(root)}{CLR_RESET}")
            modified += 1

    if modified > 0:
        print(f"\n{CLR_GREEN}[HOTOVO]{CLR_RESET} Automaticky ošetřeno {modified} souborů (Autor: {author}).")
    else:
        print(f"{CLR_GREEN}[OK]{CLR_RESET} Všechny soubory již mají platnou Doxygen hlavičku.")

if __name__ == "__main__":
    main()