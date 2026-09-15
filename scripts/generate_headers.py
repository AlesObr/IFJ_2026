#!/usr/bin/env python3
"""
scripts/generate_headers.py
Inženýrský generátor a refaktoring Doxygen hlaviček a include guardů pro IFJ26:
- Bezpečně detekuje, odstřihne a nahradí starou/nevalidní hlavičku.
- Kumuluje autory (@author), aby nepřemazal původního tvůrce.
- Zachovává ručně upravený @brief při regeneraci.
- Cyklicky odstraňuje zdvojené include guardy a nasazuje striktně jeden.
- Podporuje --force pro totální regeneraci.
"""

import sys
import re
import subprocess
from pathlib import Path

CLR_RESET  = "\033[0m"
CLR_GREEN  = "\033[38;5;46m"
CLR_YELLOW = "\033[38;5;220m"
CLR_CYAN   = "\033[38;5;51m"

VALID_EXTS = {".c", ".h"}

HEADER_COMMENT_REGEX = re.compile(r"^\s*/\*\*.*?\*/\s*", re.DOTALL)

GUARD_START_RE = re.compile(
	r"^\s*#ifndef\s+([a-zA-Z0-9_]+)\s*\n\s*#define\s+\1[^\n]*\n?",
	re.MULTILINE
)

GUARD_END_RE = re.compile(
	r"\n\s*#endif(?:\s*/\*.*?\*/|\s*//[^\n]*)?\s*$"
)

def get_git_author() -> str:
	try:
		name = subprocess.check_output(["git", "config", "user.name"], text=True).strip()
		email = subprocess.check_output(["git", "config", "user.email"], text=True).strip()
		login = email.split("@")[0] if "@" in email else email
		return f"{name} ({login})"
	except Exception:
		return "IFJ26 Team Developer"

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
		"codegen": f"Target code generator {role} emitting IFJcode26 instructions.",
		"ast": f"Abstract syntax tree data structures and {role}.",
		"str": f"Dynamic string buffer utility {role}.",
		"arena": f"Region-based memory arena allocator {role}.",
		"error": f"Error codes and diagnostic reporter {role}.",
		"main": "Compiler entry point and pipeline orchestration.",
	}
	return descriptions.get(stem, f"{filepath.stem.replace('_', ' ').capitalize()} module {role} for IFJ26 compiler.")

def extract_existing_authors(header_text: str) -> list[str]:
	authors = []
	for line in header_text.splitlines():
		match = re.search(r"@author\s+(.+)$", line)
		if match:
			authors.append(match.group(1).strip())
	return authors

def generate_header(filepath: Path, authors: list[str], custom_brief: str | None = None) -> str:
	brief = custom_brief if custom_brief else derive_brief(filepath)
	author_lines = "\n".join([f" * @author {a}" for a in authors])
	return (
		"/**\n"
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
			current = current[start_match.end():end_match.start()].strip()
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

def process_file(filepath: Path, current_author: str, force: bool = False) -> bool:
	raw_content = filepath.read_text(encoding="utf-8")

	match = HEADER_COMMENT_REGEX.search(raw_content)
	existing_brief = None
	authors = [current_author]

	if match and match.start() < 200:
		existing_header = match.group(0)
		body = (raw_content[:match.start()] + raw_content[match.end():]).strip()

		existing_brief = extract_existing_brief(existing_header)
		found_authors = extract_existing_authors(existing_header)
		for a in found_authors:
			if a not in authors:
				authors.append(a)

		is_valid = (
			f"@file {filepath.name}" in existing_header
			and "@author" in existing_header
			and "@brief" in existing_header
			and "@note Standard: ISO C11" in existing_header
		)

		if is_valid and not force and current_author in found_authors:
			if filepath.suffix != ".h":
				return False
			guard_name = f"IFJ26_{filepath.stem.upper()}_H"
			stripped_body = body.strip()
			start_match = GUARD_START_RE.match(stripped_body)
			end_match = GUARD_END_RE.search(stripped_body)
			if start_match and end_match and start_match.group(1) == guard_name:
				inner = stripped_body[start_match.end():end_match.start()].strip()
				if not GUARD_START_RE.search(inner) and not GUARD_END_RE.search(inner):
					return False
	else:
		body = raw_content.strip()

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
		print(f"Adresář {src_dir} nenalezen.")
		sys.exit(1)

	force = "--force" in sys.argv or "-f" in sys.argv
	raw_args = [a for a in sys.argv[1:] if a not in ("--force", "-f")]

	author = get_git_author()

	if raw_args:
		files = [Path(arg).resolve() for arg in raw_args if Path(arg).suffix in VALID_EXTS]
	else:
		files = sorted([p for p in src_dir.rglob("*") if p.suffix in VALID_EXTS])

	modified = 0
	for f in files:
		if process_file(f, author, force=force):
			print(f"  {CLR_GREEN}✔{CLR_RESET} Aktualizována hlavička -> {CLR_CYAN}{f.relative_to(root)}{CLR_RESET}")
			modified += 1

	if modified > 0:
		print(f"\n{CLR_GREEN}[HOTOVO]{CLR_RESET} Upraveno {modified} souborů.")
	else:
		print(f"{CLR_GREEN}[OK]{CLR_RESET} Všechny soubory jsou 100% synchronní se standardem.")

if __name__ == "__main__":
	main()