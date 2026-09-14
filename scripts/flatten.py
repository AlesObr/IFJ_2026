#!/usr/bin/env python3
"""
scripts/pack.py
Zplošťovač stromové struktury src/ do plochého adresáře a zabalení archivu IFJ26.
- Zploští hierarchii do build/submission/
- Přepíše relativní #include cesty na ploché
- Otestuje kompilaci na plochém stavu
- Vygeneruje odevzdávací tar.gz archiv
"""

import os
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
	makefile_content = (
		"CC = gcc\n"
		"CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic\n"
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

def main():
	root = find_project_root()
	src_dir = root / "src"
	dist_dir = root / "build" / "submission"
	
	author_login = sys.argv[1] if len(sys.argv) > 1 else "xwittk00"
	archive_name = f"{author_login}.tgz"

	print(f"{CLR_CYAN}{CLR_BOLD}[1/4] Příprava plochého adresáře:{CLR_RESET} {dist_dir}")
	if dist_dir.exists():
		shutil.rmtree(dist_dir)
	dist_dir.mkdir(parents=True, exist_ok=True)

	print(f"{CLR_CYAN}{CLR_BOLD}[2/4] Zplošťování zdrojáků a přepis #include...{CLR_RESET}")
	flatten_and_rewrite(src_dir, dist_dir)
	generate_flat_makefile(dist_dir)

	# Přiložit dokumentaci, pokud existuje
	doc_file = root / "dokumentace.pdf"
	if doc_file.exists():
		shutil.copy(doc_file, dist_dir / "dokumentace.pdf")
		print(f"  {CLR_GREEN}✔{CLR_RESET} Přiložena dokumentace.pdf")
	else:
		print(f"  {CLR_YELLOW}⚠ dokumentace.pdf nenalezena (bude potřeba před odevzdáním){CLR_RESET}")

	print(f"{CLR_CYAN}{CLR_BOLD}[3/4] Zkouška kompilace ploché verze (make)...{CLR_RESET}")
	try:
		subprocess.run(["make", "clean"], cwd=dist_dir, check=True, stdout=subprocess.DEVNULL)
		subprocess.run(["make"], cwd=dist_dir, check=True)
		print(f"  {CLR_GREEN}✔ Kompilace v plochém stavu proběhla bez jediného warningu.{CLR_RESET}")
		subprocess.run(["make", "clean"], cwd=dist_dir, check=True, stdout=subprocess.DEVNULL)
	except subprocess.CalledProcessError:
		print(f"{CLR_RED}[FATAL] Ploché sestavení selhalo! Odevzdání přerušeno.{CLR_RESET}")
		sys.exit(1)

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