#!/usr/bin/env python3
"""
scripts/lint_comments.py
Complex linter for C/H source files in the IFJ26 project. It checks for:
- Doxygen header presence and correctness
- Comment hygiene (no vulgarisms, no Czech/Slovak diacritics, no commented-out code)
- Indentation and whitespace issues (no spaces for indentation, no trailing spaces)
- Header file rules (no function bodies in .h, Doxygen comments for declarations)
- Stripping and managing temporary development comments (@dev)
"""

import sys
import os
import re
import subprocess
from pathlib import Path

# ANSI barevná paleta
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

# Zákaz vulgarismů a kompromitujících termínů (VUL / Inkvizice)
BANNED_REGEX = re.compile(
	r"\b(TODO|FIXME|HACK|GHIDRA|IC26INT|KRIVKA|KŘIVKA|COPILOT|CHATGPT|CLAUDE|LLM|PROMPT|DO_PRDELE|KURV|HOVNO|DEBIL|KOKOT)\b",
	re.IGNORECASE
)

# Detekce CZ/SK diakritiky
CZECH_CHARS_RE = re.compile(r"[áčďéěíňóřšťúůýžÁČĎÉĚÍŇÓŘŠŤÚŮÝŽ]")

# Detekce 4+ mezer na začátku řádku (porušení tabulátorové doktríny)
SPACE_INDENT_REGEX = re.compile(r"^( {4,})")

# Detekce těla funkce v .h souboru (vyjma povolených static inline helperů)
FUNC_BODY_IN_HEADER_RE = re.compile(
	r"^(?!\s*(?:typedef|struct|enum|union|if|while|for|switch|(?:static\s+)?inline))\s*[a-zA-Z_][a-zA-Z0-9_* \t]+\s*\([^;)]*\)\s*\{"
)

# Deklarace nebo inline definice funkce v .h pro kontrolu Doxygenu
FUNC_IN_HEADER_RE = re.compile(
	r"^\s*(?!\s*(?:typedef|struct|enum|union|return))\s*(?:static\s+)?(?:inline\s+)?([a-zA-Z_][a-zA-Z0-9_* \t]+)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\([^)]*\)\s*[{;]"
)

# Definice ADT (struct, enum, union) v .h souboru
ADT_DEF_IN_HEADER_RE = re.compile(
	r"^\s*(?:typedef\s+)?(?:struct|enum|union)(?:\s+[a-zA-Z_][a-zA-Z0-9_]*)?\s*\{"
)

# Detekce zakomentovaného C kódu (COMCODE)
COMMENTED_CODE_RE = re.compile(
	r"(?:\b(?:if|while|for|return|switch|case)\b.*[;{]|\b(?:int|char|double|float|void|size_t|Token)\b\s+[*a-zA-Z0-9_]+.*[;=]|\b[a-zA-Z_][a-zA-Z0-9_]*\s*=\s*[^;]+;)"
)

# Detekce globálních proměnných v .c souborech
GLOBAL_VAR_DECL_RE = re.compile(
	r"^(?!\s*(?:static|extern|typedef|/\*|\*|//))\s*"
	r"((?:const\s+)?(?:unsigned\s+|signed\s+)?(?:struct\s+[a-zA-Z_][a-zA-Z0-9_*]*|[a-zA-Z_][a-zA-Z0-9_]*)\s*[* \t]+)"
	r"([a-zA-Z_][a-zA-Z0-9_]*)"
	r"(?:\s*\[[^\]]*\])*"
	r"(?:\s*=\s*[^;]+)?\s*;"
)

SNAKE_CASE_RE = re.compile(r"^[a-z][a-z0-9_]*$")
UPPER_CASE_MACRO_RE = re.compile(r"^[A-Z][A-Z0-9_]*$")

FUNC_NAME_RE = re.compile(
	r"^(?!\s*(?:if|while|for|switch|return))\s*"
	r"(?:static\s+|inline\s+)*"
	r"[a-zA-Z_][a-zA-Z0-9_* \t]+\s+([a-zA-Z0-9_]+)\s*\([^;)]*\)\s*\{?"
)

VAR_DECL_RE = re.compile(
	r"^\s*(?:static\s+|const\s+|volatile\s+|unsigned\s+|signed\s+)*"
	r"(?:void|int|char|short|long|float|double|size_t|ssize_t|bool|uint\d+_t|int\d+_t|struct\s+[a-zA-Z0-9_]+|[a-zA-Z0-9_]+_t)\s+"
	r"[* \t]*([a-zA-Z0-9_]+)"
	r"(?:\s*\[[^\]]*\])*"
	r"\s*(?:=|;|,)"
)

KEYWORDS_BLACKLIST = {
	"if", "while", "for", "switch", "case", "default", "return",
	"break", "continue", "goto", "sizeof", "NULL", "true", "false"
}


def format_loc(filepath: Path, line: int, col: int) -> str:
	return f"{CLR_CYAN}{filepath}{CLR_RESET}:{CLR_YELLOW}{line}{CLR_RESET}:{CLR_MAGENTA}{col}{CLR_RESET}"


def find_project_root() -> Path:
	current = Path(__file__).resolve().parent
	while current != current.parent:
		if (current / ".git").exists() or (current / "Makefile").exists():
			return current
		current = current.parent
	return Path(__file__).resolve().parent.parent


def is_boss_override_active(root: Path) -> bool:
	if os.environ.get("BOSS_OVERRIDE") == "1" or os.environ.get("IFJ_OVERRIDE") == "WITTKE_APPROVED":
		return True

	try:
		msg = subprocess.check_output(
			["git", "log", "-1", "--pretty=%B"],
			cwd=root, text=True, stderr=subprocess.DEVNULL
		)
		if "[override: wittke]" in msg.lower() or "[boss override]" in msg.lower():
			return True
	except Exception:
		pass

	return False


def strip_dev_comments(filepath: Path) -> bool:
	"""Odstraní jednořádkové i víceřádkové komentáře se značkou @dev."""
	try:
		content = filepath.read_text(encoding="utf-8")
	except UnicodeDecodeError:
		return False

	original = content

	# 1. Odstranění víceřádkových bloků /* @dev ... */
	content = re.sub(r"/\*+[ \t]*@dev\b.*?\*/[ \t]*\r?\n?", "", content, flags=re.DOTALL)

	# 2. Odstranění jednořádkových // @dev ... (samostatný řádek)
	content = re.sub(r"^[ \t]*//[ \t]*@dev\b.*?\r?\n", "", content, flags=re.MULTILINE)

	# 3. Odstranění inline // @dev ... na konci platného řádku kódu
	content = re.sub(r"[ \t]*//[ \t]*@dev\b.*$", "", content, flags=re.MULTILINE)

	if content != original:
		filepath.write_text(content, encoding="utf-8")
		return True
	return False


def fix_indentation_and_whitespace(filepath: Path) -> bool:
	try:
		content = filepath.read_text(encoding="utf-8")
	except UnicodeDecodeError:
		return False

	lines = content.splitlines(keepends=True)
	modified = False
	new_lines = []

	for line in lines:
		newline_match = re.search(r"(\r?\n)$", line)
		ending = newline_match.group(1) if newline_match else ""
		raw_line = line[:len(line) - len(ending)]

		rstripped = raw_line.rstrip(" \t")
		if rstripped != raw_line:
			modified = True
			raw_line = rstripped

		leading_spaces_match = re.match(r"^([ \t]+)", raw_line)
		if leading_spaces_match:
			prefix = leading_spaces_match.group(1)
			if "    " in prefix:
				tabs_count = 0
				spaces_acc = 0
				for ch in prefix:
					if ch == '\t':
						tabs_count += 1
					elif ch == ' ':
						spaces_acc += 1
						if spaces_acc == 4:
							tabs_count += 1
							spaces_acc = 0

				new_prefix = ('\t' * tabs_count) + (' ' * spaces_acc)
				if new_prefix != prefix:
					raw_line = new_prefix + raw_line[len(prefix):]
					modified = True

		new_lines.append(raw_line + ending)

	if modified:
		filepath.write_text("".join(new_lines), encoding="utf-8")

	return modified


def check_indentation_and_whitespace(filepath: Path, lines: list[str]) -> list[str]:
	errors = []
	for idx, line in enumerate(lines, start=1):
		match = SPACE_INDENT_REGEX.match(line)
		if match:
			cnt = len(match.group(1))
			loc = format_loc(filepath, idx, 1)
			errors.append(
				f"{loc} {CLR_RED}[INDENT]{CLR_RESET} Nalezeno {cnt} mezer pro odsazení. Používej výhradně tabulátory (\\t)."
			)

		if line.endswith(" ") or line.endswith("\t"):
			loc = format_loc(filepath, idx, len(line))
			errors.append(
				f"{loc} {CLR_YELLOW}[WHITESPACE]{CLR_RESET} Odstraň prázdné znaky na konci řádku."
			)

	return errors


def check_file_doxygen_header(filepath: Path, lines: list[str]) -> list[str]:
	errors = []
	if not lines or not lines[0].strip().startswith("/**"):
		loc = format_loc(filepath, 1, 1)
		errors.append(f"{loc} {CLR_RED}[NOHEAD]{CLR_RESET} Chybí zahájení Doxygen bloku '/**' na řádku 1.")
		return errors

	header_block = "\n".join(lines[:15])

	if f"@file {filepath.name}" not in header_block:
		loc = format_loc(filepath, 2, 1)
		errors.append(f"{loc} {CLR_RED}[NOHEAD]{CLR_RESET} V hlavičce chybí přesný název: '{CLR_BOLD}@file {filepath.name}{CLR_RESET}'")

	if "@author" not in header_block:
		loc = format_loc(filepath, 1, 1)
		errors.append(f"{loc} {CLR_RED}[NOAUTHR]{CLR_RESET} V hlavičce chybí jméno autora (@author).")

	for tag in ["@brief", "@note Standard: ISO C11"]:
		if tag not in header_block:
			loc = format_loc(filepath, 1, 1)
			errors.append(f"{loc} {CLR_RED}[NOHEAD]{CLR_RESET} V hlavičce chybí povinná položka: '{CLR_YELLOW}{tag}{CLR_RESET}'")

	return errors


def check_header_file_rules(filepath: Path, lines: list[str]) -> list[str]:
	errors = []
	if filepath.suffix != ".h":
		return errors

	for idx, line in enumerate(lines, start=1):
		stripped = line.strip()

		if not stripped or stripped.startswith("#"):
			continue

		if FUNC_BODY_IN_HEADER_RE.search(stripped):
			loc = format_loc(filepath, idx, 1)
			errors.append(
				f"{loc} {CLR_RED}[HDRCODE]{CLR_RESET} Tělo běžné funkce implementováno v .h souboru! "
				f"Patří do .c modulu (případně použij 'static inline')."
			)

		is_func = bool(FUNC_IN_HEADER_RE.match(line))
		is_adt_def = bool(ADT_DEF_IN_HEADER_RE.match(stripped))

		if is_func or is_adt_def:
			entity_type = "Funkce" if is_func else "ADT (struct/enum/union)"
			prev_idx = idx - 2
			while prev_idx >= 0 and not lines[prev_idx].strip():
				prev_idx -= 1

			has_doxygen = False
			if prev_idx >= 0 and lines[prev_idx].strip().endswith("*/"):
				walk = prev_idx
				while walk >= 0:
					wline = lines[walk].strip()
					if "/**" in wline:
						has_doxygen = True
						break
					if "/*" in wline:
						break
					walk -= 1

			if not has_doxygen:
				loc = format_loc(filepath, idx, 1)
				errors.append(
					f"{loc} {CLR_YELLOW}[NOFNCOM]{CLR_RESET} {entity_type} postrádá dokumentační blok (/** ... */)!"
				)

	return errors


def check_comments_hygiene(filepath: Path, content: str) -> list[str]:
	errors = []
	lines = content.splitlines()
	in_block = False
	in_file_header = False
	in_dev_block = False

	for idx, line in enumerate(lines, start=1):
		stripped = line.strip()

		if idx == 1 and stripped.startswith("/**"):
			in_file_header = True

		if in_block:
			if "*/" in line:
				in_block = False
				in_dev_block = False
				if in_file_header:
					in_file_header = False
			comment_text = line
			if in_dev_block:
				continue
		elif "/*" in line:
			in_block = True
			comment_text = line.split("/*", 1)[1]
			if "@dev" in comment_text:
				in_dev_block = True
			if "*/" in line:
				in_block = False
				in_dev_block = False
				if in_file_header:
					in_file_header = False
			if in_dev_block:
				continue
		elif "//" in line:
			comment_text = line.split("//", 1)[1]
			if "@dev" in comment_text:
				continue
		else:
			continue

		loc = format_loc(filepath, idx, 1)

		# VUL: Zakázaná slova se kontrolují mimo @dev všude
		match = BANNED_REGEX.search(comment_text)
		if match:
			banned_word = match.group(0)
			errors.append(
				f"{loc} {CLR_RED}{CLR_BOLD}[VUL/BEZPEČNOST]{CLR_RESET} Zakázaný výraz '{CLR_RED}{banned_word}{CLR_RESET}' v komentáři!"
			)

		if in_file_header and "@author" in comment_text:
			continue

		# MIXLANG: Kontrola diakritiky
		if CZECH_CHARS_RE.search(comment_text):
			snippet = comment_text.strip()
			if len(snippet) > 40:
				snippet = snippet[:37] + "..."
			errors.append(
				f"{loc} {CLR_YELLOW}[MIXLANG]{CLR_RESET} Nalezena diakritika: \"{CLR_GRAY}{snippet}{CLR_RESET}\". Používej výhradně angličtinu nebo značku @dev."
			)

		# COMCODE: Detekce zakomentovaného kódu
		if not in_file_header and "@" not in comment_text and "Standard:" not in comment_text:
			if COMMENTED_CODE_RE.search(comment_text.strip()):
				snippet = comment_text.strip()
				if len(snippet) > 40:
					snippet = snippet[:37] + "..."
				errors.append(
					f"{loc} {CLR_YELLOW}[COMCODE]{CLR_RESET} Podezření na zakomentovaný kód: \"{CLR_GRAY}{snippet}{CLR_RESET}\""
				)

	return errors


def check_unjustified_globals(filepath: Path, lines: list[str]) -> list[str]:
	errors = []
	if filepath.suffix != ".c":
		return errors

	brace_depth = 0
	in_block_comment = False

	for idx, line in enumerate(lines, start=1):
		stripped = line.strip()

		if in_block_comment:
			if "*/" in stripped:
				in_block_comment = False
			continue
		if "/*" in stripped and "*/" not in stripped:
			in_block_comment = True
			continue

		if not stripped or stripped.startswith(("#", "//", "/*", "*")):
			continue

		clean_code = re.sub(r'".*?"|\'.*?\'', '', stripped)

		if brace_depth == 0:
			match = GLOBAL_VAR_DECL_RE.match(stripped)
			if match and "(" not in stripped.split(";")[0]:
				var_name = match.group(2)
				prev_idx = idx - 2
				has_global_justification = False

				while prev_idx >= 0:
					prev_line = lines[prev_idx].strip()
					if not prev_line:
						prev_idx -= 1
						continue
					if "@global" in prev_line:
						has_global_justification = True
						break
					if not prev_line.startswith(("//", "/*", "*")):
						break
					prev_idx -= 1

				if not has_global_justification:
					loc = format_loc(filepath, idx, 1)
					errors.append(
						f"{loc} {CLR_YELLOW}[GP]{CLR_RESET} Nestatická globální proměnná '{var_name}' bez odůvodnění! "
						f"Použij 'static' nebo přidej nad definici komentář '// @global: duvod'."
					)

		brace_depth += clean_code.count("{") - clean_code.count("}")
		if brace_depth < 0:
			brace_depth = 0

	return errors


def check_naming_conventions(filepath: Path, lines: list[str]) -> list[str]:
	errors = []

	for idx, line in enumerate(lines, start=1):
		stripped = line.strip()

		if not stripped or stripped.startswith(("#", "//", "/*", "*")):
			continue

		if stripped.startswith(("typedef", "struct ", "enum ", "union ")):
			continue

		func_match = FUNC_NAME_RE.match(stripped)
		if func_match:
			func_name = func_match.group(1)
			if func_name not in KEYWORDS_BLACKLIST and not UPPER_CASE_MACRO_RE.match(func_name):
				if not SNAKE_CASE_RE.match(func_name):
					col = line.find(func_name) + 1
					loc = format_loc(filepath, idx, col)
					errors.append(
						f"{loc} {CLR_YELLOW}[NAMING]{CLR_RESET} Funkce '{func_name}' není ve snake_case!"
					)
			continue

		var_match = VAR_DECL_RE.match(line)
		if var_match:
			var_name = var_match.group(1)
			if var_name in KEYWORDS_BLACKLIST or UPPER_CASE_MACRO_RE.match(var_name):
				continue

			if not SNAKE_CASE_RE.match(var_name):
				col = line.find(var_name) + 1
				loc = format_loc(filepath, idx, col)
				errors.append(
					f"{loc} {CLR_YELLOW}[NAMING]{CLR_RESET} Proměnná '{var_name}' není ve snake_case!"
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

	errors.extend(check_file_doxygen_header(filepath, lines))
	errors.extend(check_indentation_and_whitespace(filepath, lines))
	errors.extend(check_header_file_rules(filepath, lines))
	errors.extend(check_comments_hygiene(filepath, content))
	errors.extend(check_unjustified_globals(filepath, lines))
	errors.extend(check_naming_conventions(filepath, lines))

	return errors


def main():
	root = find_project_root()
	src_dir = root / "src"

	if not src_dir.exists():
		print(f"\n{CLR_RED}{CLR_BOLD}[FATAL]{CLR_RESET} Adresář 'src' nenalezen v {CLR_CYAN}{root}{CLR_RESET}\n")
		sys.exit(2)

	auto_fix = "--fix" in sys.argv or "-f" in sys.argv
	strip_dev = "--strip-dev" in sys.argv or "-s" in sys.argv
	args = [arg for arg in sys.argv[1:] if arg not in ("--fix", "-f", "--strip-dev", "-s")]

	if args:
		raw_files = [Path(arg).resolve() for arg in args]
		files_to_check = [
			f for f in raw_files
			if f.suffix in VALID_EXTS and f.exists() and src_dir in f.parents
		]
	else:
		files_to_check = sorted([
			p for p in src_dir.rglob("*")
			if p.is_file() and p.suffix in VALID_EXTS
		])

	# 1. Vyčištění interních vývojářských poznámek před odevzdáním
	if strip_dev:
		stripped_count = 0
		for filepath in files_to_check:
			if strip_dev_comments(filepath):
				stripped_count += 1
				print(f"  {CLR_MAGENTA}[STRIPPED @dev]{CLR_RESET} {filepath.relative_to(root)}")
		if stripped_count > 0:
			print(f"{CLR_MAGENTA}{CLR_BOLD}[CLEANUP]{CLR_RESET} Odstraněny interní poznámky v {stripped_count} souborech.\n")

	# 2. Automatická oprava mezer / tabulátorů
	if auto_fix:
		fixed_count = 0
		for filepath in files_to_check:
			if fix_indentation_and_whitespace(filepath):
				fixed_count += 1
				print(f"  {CLR_GREEN}[FIXED]{CLR_RESET} {filepath.relative_to(root)}")
		if fixed_count > 0:
			print(f"{CLR_GREEN}{CLR_BOLD}[AUTO-FIX]{CLR_RESET} Úspěšně opraveno odsazení v {fixed_count} souborech.\n")

	total_errors = []
	for filepath in files_to_check:
		total_errors.extend(lint_file(filepath))

	if total_errors:
		if is_boss_override_active(root):
			print(f"\n{CLR_MAGENTA}{CLR_BOLD}╔{'═' * 74}╗{CLR_RESET}")
			print(f"{CLR_MAGENTA}{CLR_BOLD}║  [BOSS OVERRIDE AKTIVOVÁN] NALEZENY CHYBY, ALE ŠÉF UDĚLIL VÝJIMKU!         ║{CLR_RESET}")
			print(f"{CLR_MAGENTA}{CLR_BOLD}╚{'═' * 74}╝{CLR_RESET}\n")
			for err in total_errors:
				print(f"  {CLR_YELLOW}⚠ (POTLAČENO){CLR_RESET} {err}")
			print(f"\n{CLR_GREEN}{CLR_BOLD}Výjimka platná. Kód prochází dál.{CLR_RESET}\n")
			sys.exit(0)

		print(f"\n{CLR_RED}{CLR_BOLD}╔{'═' * 74}╗{CLR_RESET}")
		print(f"{CLR_RED}{CLR_BOLD}║  [INKVIZICE] AUDIT KÓDU SELHAL: NALEZENO {len(total_errors):02d} CHYB                             ║{CLR_RESET}")
		print(f"{CLR_RED}{CLR_BOLD}╚{'═' * 74}╝{CLR_RESET}\n")

		for err in total_errors:
			print(f"  {CLR_RED}✖{CLR_RESET} {err}")

		print(f"\n{CLR_YELLOW}{CLR_BOLD}Přehled penalizací z ceníku IFJ:{CLR_RESET}")
		print(f"  {CLR_BLUE}• INDENT  (-0,05 b):{CLR_RESET} Odsazení pouze tabulátory (\\t), žádné mezery.")
		print(f"  {CLR_BLUE}• NOHEAD  (-0,50 b):{CLR_RESET} Chybí Doxygen záhlaví na řádku 1.")
		print(f"  {CLR_BLUE}• NOAUTHR (-0,20 b):{CLR_RESET} Chybí autor v záhlaví.")
		print(f"  {CLR_BLUE}• HDRCODE (-0,30 b):{CLR_RESET} Těla funkcí v .h souborech (struct/enum povoleny).")
		print(f"  {CLR_BLUE}• NOFNCOM (-0,20 b):{CLR_RESET} Chybí /** ... */ nad funkcemi nebo ADT v .h.")
		print(f"  {CLR_BLUE}• MIXLANG (-0,10 b):{CLR_RESET} Čeština/slovenština v komentářích (vyjma @author a @dev).")
		print(f"  {CLR_BLUE}• COMCODE (-0,05 b):{CLR_RESET} Zakomentované bloky zdrojového kódu.")
		print(f"  {CLR_BLUE}• GP      (-0,10 b):{CLR_RESET} Nezdůvodněné nestatické globální proměnné.")
		print(f"  {CLR_BLUE}• VUL     (-0,30 b):{CLR_RESET} Zakázaná slova, vulgarismy a zmínky o AI.")
		print(f"\n{CLR_CYAN}Tip:{CLR_RESET} Použij '{CLR_BOLD}--fix{CLR_RESET}' pro odsazení nebo '{CLR_BOLD}--strip-dev{CLR_RESET}' pro smazání @dev poznámek před odevzdáním.\n")
		sys.exit(1)

	print(f"{CLR_GREEN}{CLR_BOLD}[OK]{CLR_RESET} Zkontrolováno {CLR_CYAN}{len(files_to_check)}{CLR_RESET} souborů (.c/.h) v {CLR_DIM}src/**{CLR_RESET} — {CLR_GREEN}100% sterilní, nulová ztráta bodů.{CLR_RESET}")
	sys.exit(0)


if __name__ == "__main__":
	main()