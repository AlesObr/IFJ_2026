#!/usr/bin/env python3
"""
scripts/flatten.py
Flatten and package IFJ26 compiler source files for submission. It copies all .c and .h files from src/ into a single directory, rewrites #include directives to use only filenames, generates a Makefile, verifies that the flat sources build without warnings and optionally creates a .tgz archive.
Usage:
	python3 scripts/flatten.py [xlogin]                     # build/submission + xlogin.tgz
	python3 scripts/flatten.py --out build/dist --no-archive # used by `make zip`
	The login defaults to the team leader from team.json.

AI has been used to generate the initial version of this script, but it has been reviewed and modified by Aleš Obr (xobrale00) to ensure accuracy and adherence to project standards.
"""

import argparse
import json
import sys
import re
import shutil
import subprocess
from pathlib import Path

# ANSI barvy
CLR_RESET  = "\033[0m"
CLR_BOLD   = "\033[1m"
CLR_GREEN  = "\033[38;5;46m"
CLR_RED    = "\033[38;5;196m"
CLR_YELLOW = "\033[38;5;220m"
CLR_CYAN   = "\033[38;5;51m"

INCLUDE_REGEX = re.compile(r'#\s*include\s*"([^"]+)"')
# Soubory, které musí být v kořeni odevzdaného archivu vedle zdrojáků a Makefile
SUBMISSION_FILES = ("dokumentace.pdf", "rozdeleni", "rozsireni")

def find_project_root() -> Path:
	current = Path(__file__).resolve().parent
	while current != current.parent:
		if (current / ".git").exists() or (current / "Makefile").exists():
			return current
		current = current.parent
	return Path(__file__).resolve().parent.parent

def flatten_and_rewrite(src_dir: Path, target_dir: Path):
	"""Zkopíruje soubory do jedné úrovně a přepíše include cesty."""
	files = [p for p in src_dir.rglob("*") if p.is_file() and p.suffix in {".c", ".h"}]
	seen_names = {}

	# Detekce kolizí jmen
	for f in files:
		if f.name in seen_names:
			print(f"{CLR_RED}[KOLIZE]{CLR_RESET} Dva soubory mají stejné jméno v různých podsložkách: {f.name}")
			print(f"  - {seen_names[f.name]}")
			print(f"  - {f}")
			sys.exit(1)
		seen_names[f.name] = f

	# Kopírování a transformace
	for f in files:
		dest = target_dir / f.name
		content = f.read_text(encoding="utf-8")

		def replace_include(match):
			inc_path = match.group(1)
			inc_filename = Path(inc_path).name
			return f'#include "{inc_filename}"'

		rewritten = INCLUDE_REGEX.sub(replace_include, content)
		dest.write_text(rewritten, encoding="utf-8")

def generate_flat_makefile(target_dir: Path):
	"""Vytvoří jednoduchý Make target pro zploštělou strukturu."""
	# Bez -Werror: jiná verze gcc na hodnoticím serveru by jinak mohla zastavit překlad.
	# Varování hlídá zkušební překlad v main() (CFLAGS s -Werror).
	makefile_content = (
		"CC = gcc\n"
		"CFLAGS = -std=c11 -Wall -Wextra -pedantic\n"
		"SRCS = $(wildcard *.c)\n"
		"OBJS = $(SRCS:.c=.o)\n"
		"TARGET = ifj26\n\n"
		"all: $(TARGET)\n\n"
		"$(TARGET): $(OBJS)\n"
		"\t$(CC) $(CFLAGS) -o $@ $^\n\n"
		"%.o: %.c\n"
		"\t$(CC) $(CFLAGS) -c $< -o $@\n\n"
		"clean:\n"
		"\trm -f $(OBJS) $(TARGET)\n\n"
		".PHONY: all clean\n"
	)
	(target_dir / "Makefile").write_text(makefile_content, encoding="utf-8")

def default_login(root: Path) -> str | None:
	"""Login vedoucího z team.json (klíč "leader", jinak první člen)."""
	try:
		team = json.loads((root / "team.json").read_text(encoding="utf-8"))
	except (OSError, ValueError):
		return None
	if team.get("leader"):
		return team["leader"]
	members = team.get("members") or []
	return members[0].get("login") if members else None

def main():
	root = find_project_root()
	src_dir = root / "src"

	parser = argparse.ArgumentParser(description="Zploštění zdrojáků IFJ26 pro odevzdání")
	parser.add_argument("login", nargs="?", help="Login pro název archivu (výchozí: vedoucí z team.json)")
	parser.add_argument("--out", type=Path, default=root / "build" / "submission",
		help="Cílový adresář (výchozí build/submission); jeho obsah se přepíše")
	parser.add_argument("--no-archive", action="store_true",
		help="Nevytvářet .tgz (archiv pak vytváří např. `make zip`)")
	args = parser.parse_args()

	dist_dir = args.out if args.out.is_absolute() else Path.cwd() / args.out
	author_login = args.login or default_login(root)
	if not args.no_archive and not author_login:
		print(f"{CLR_RED}[FATAL] Chybí login: zadejte ho jako argument nebo doplňte team.json.{CLR_RESET}")
		sys.exit(1)
	archive_name = f"{author_login}.tgz"
	# Ochrana: obsah cílového adresáře se maže, nesmí to být zdrojáky ani kořen projektu
	resolved = dist_dir.resolve()
	if resolved in (root, src_dir) or root.resolve() not in resolved.parents:
		print(f"{CLR_RED}[FATAL] Cílový adresář musí ležet uvnitř projektu a mimo src/: {dist_dir}{CLR_RESET}")
		sys.exit(1)

	print(f"{CLR_CYAN}{CLR_BOLD}[1/4] Příprava plochého adresáře:{CLR_RESET} {dist_dir}")
	if dist_dir.exists():
		shutil.rmtree(dist_dir)
	dist_dir.mkdir(parents=True, exist_ok=True)

	print(f"{CLR_CYAN}{CLR_BOLD}[2/4] Zplošťování zdrojáků a přepis #include...{CLR_RESET}")
	flatten_and_rewrite(src_dir, dist_dir)
	generate_flat_makefile(dist_dir)

	# Povinné soubory odevzdání (kontroluje je is_it_ok.sh); hledají se v kořeni a v src/
	for name in SUBMISSION_FILES:
		source = next((d / name for d in (root, src_dir) if (d / name).is_file()), None)
		if source is not None:
			shutil.copy(source, dist_dir / name)
			print(f"  {CLR_GREEN}✔{CLR_RESET} Přiložen soubor {name} ({source.relative_to(root)})")
		else:
			print(f"  {CLR_YELLOW}⚠ {name} nenalezen v kořeni ani v src/ (bude potřeba před odevzdáním){CLR_RESET}")

	print(f"{CLR_CYAN}{CLR_BOLD}[3/4] Zkouška kompilace ploché verze (make s -Werror)...{CLR_RESET}")
	try:
		subprocess.run(["make", "clean"], cwd=dist_dir, check=True, stdout=subprocess.DEVNULL)
		subprocess.run(["make", "CFLAGS=-std=c11 -Wall -Wextra -Werror -pedantic"], cwd=dist_dir, check=True)
		print(f"  {CLR_GREEN}✔ Kompilace v plochém stavu proběhla bez jediného warningu.{CLR_RESET}")
		subprocess.run(["make", "clean"], cwd=dist_dir, check=True, stdout=subprocess.DEVNULL)
	except subprocess.CalledProcessError:
		print(f"{CLR_RED}[FATAL] Ploché sestavení selhalo! Odevzdání přerušeno.{CLR_RESET}")
		sys.exit(1)

	if args.no_archive:
		print(f"\n{CLR_GREEN}{CLR_BOLD}[HOTOVO]{CLR_RESET} Zploštěné zdrojáky jsou v {dist_dir} (archiv nevytvářen).")
		return

	print(f"{CLR_CYAN}{CLR_BOLD}[4/4] Vytváření finálního archivu:{CLR_RESET} {archive_name}")
	archive_path = root / archive_name
	
	# IFJ odevzdává obsah přímo v kořenu archivu
	subprocess.run(
		["tar", "-czf", str(archive_path), "-C", str(dist_dir), "."],
		check=True
	)

	print(f"\n{CLR_GREEN}{CLR_BOLD}[HOTOVO]{CLR_RESET} Archiv {CLR_CYAN}{archive_name}{CLR_RESET} je připraven k odevzdání.")

if __name__ == "__main__":
	main()