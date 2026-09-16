#!/usr/bin/env python3
"""Grammar-based fuzzer pro IFJ26 s ukládáním do souborů."""

import argparse
import json
import random
import subprocess
import sys
from pathlib import Path

DEFAULT_GRAMMAR = {
	"<program>": [["<stmt_list>"]],
	"<stmt_list>": [
		["<stmt>", "\n", "<stmt_list>"],
		["<stmt>"]
	],
	"<stmt>": [
		["<var_decl>"],
		["<assignment>"],
		["<if_stmt>"],
		["<while_stmt>"],
		["<expr_stmt>"]
	],
	"<var_decl>": [
		["var ", "<id>", " : ", "<type>", " = ", "<expr>", ";"],
		["let ", "<id>", " = ", "<expr>", ";"]
	],
	"<assignment>": [["<id>", " = ", "<expr>", ";"]],
	"<if_stmt>": [
		["if (", "<expr>", ") {\n", "<stmt_list>", "\n} else {\n", "<stmt_list>", "\n}"]
	],
	"<while_stmt>": [
		["while (", "<expr>", ") {\n", "<stmt_list>", "\n}"]
	],
	"<expr_stmt>": [["<expr>", ";"]],
	"<expr>": [
		["<expr>", " + ", "<term>"],
		["<expr>", " - ", "<term>"],
		["<term>"]
	],
	"<term>": [
		["<term>", " * ", "<factor>"],
		["<term>", " / ", "<factor>"],
		["<factor>"]
	],
	"<factor>": [
		["(", "<expr>", ")"],
		["<id>"],
		["<literal>"]
	],
	"<type>": [["Int"], ["Float"], ["String"], ["Bool"]],
	"<id>": [["x"], ["y"], ["idx"], ["total_sum"], ["foo_bar"]],
	"<literal>": [["0"], ["42"], ["1337"], ["3.14"], ['"test"'], ["true"], ["false"]]
}


class GrammarFuzzer:
	def __init__(self, grammar: dict, max_depth: int = 15, mutation_rate: float = 0.05):
		self.grammar = grammar
		self.max_depth = max_depth
		self.mutation_rate = mutation_rate
		self.has_mutated = False
		self.terminal_depths = self._calc_terminal_depths()

	def _is_terminal(self, symbol: str) -> bool:
		return not (symbol.startswith("<") and symbol.endswith(">"))

	def _calc_terminal_depths(self) -> dict[str, int]:
		depths = {}
		changed = True
		while changed:
			changed = False
			for nt, productions in self.grammar.items():
				min_p_depth = float("inf")
				for prod in productions:
					nt_symbols = [s for s in prod if not self._is_terminal(s)]
					if not nt_symbols:
						p_depth = 1
					elif all(s in depths for s in nt_symbols):
						p_depth = 1 + max(depths[s] for s in nt_symbols)
					else:
						p_depth = float("inf")

					if p_depth < min_p_depth:
						min_p_depth = p_depth

				if min_p_depth < depths.get(nt, float("inf")):
					depths[nt] = min_p_depth
					changed = True
		return depths

	def _get_terminating_production(self, non_terminal: str) -> list[str]:
		productions = self.grammar.get(non_terminal, [])
		if not productions:
			return []

		def prod_cost(prod):
			cost = 0
			for s in prod:
				if not self._is_terminal(s):
					cost += self.terminal_depths.get(s, 999)
			return cost

		return min(productions, key=prod_cost)

	def generate(self, symbol: str = "<program>", depth: int = 0) -> str:
		if self._is_terminal(symbol):
			if self.mutation_rate > 0 and random.random() < self.mutation_rate:
				self.has_mutated = True
				mutation_type = random.choice(["drop", "duplicate", "swap_syntax"])
				if mutation_type == "drop":
					return ""
				elif mutation_type == "duplicate":
					return symbol + symbol
				elif mutation_type == "swap_syntax":
					return random.choice([";", ")", "{", "}", "=", "+"])
			return symbol

		productions = self.grammar.get(symbol)
		if not productions:
			return ""

		if depth >= self.max_depth:
			chosen_rule = self._get_terminating_production(symbol)
		else:
			chosen_rule = random.choice(productions)

		result = []
		for sym in chosen_rule:
			result.append(self.generate(sym, depth + 1))

		return "".join(result)


def run_test(compiler_bin: str, code: str, expect_syntax_error: bool) -> tuple[bool, str]:
	cmd = [
		"valgrind",
		"--leak-check=full",
		"--show-leak-kinds=all",
		"--error-exitcode=99",
		"-q",
		compiler_bin
	]

	try:
		proc = subprocess.run(
			cmd,
			input=code,
			text=True,
			capture_output=True,
			timeout=2
		)
	except subprocess.TimeoutExpired:
		return False, "TIMEOUT (nekonečný cyklus)"

	if proc.returncode == 99:
		return False, f"VALGRIND ERROR:\n{proc.stderr}"

	if proc.returncode < 0:
		return False, f"CRASH: signál {-proc.returncode}"

	if expect_syntax_error and proc.returncode != 2:
		return False, f"Očekáván exit kód 2, vráceno: {proc.returncode}"

	if not expect_syntax_error and proc.returncode not in (0, 3, 4, 5):
		return False, f"Neočekávaná syntaktická chyba: {proc.returncode}"

	return True, "OK"


def main():
	parser = argparse.ArgumentParser(description="Grammar-based syntaktický fuzzer pro IFJ26")
	parser.add_argument("--bin", type=str, default="./compiler", help="Cesta k binárce překladače")
	parser.add_argument("--count", type=int, default=50, help="Počet generovaných testů")
	parser.add_argument("--mutations", action="store_true", help="Povolit generování chybné syntaxe")
	parser.add_argument("--grammar", type=str, help="Cesta k JSON souboru s gramatikou")
	parser.add_argument("--out-dir", type=str, default="test_samples", help="Adresář pro ukládání testů")
	parser.add_argument("--save-failed-only", action="store_true", help="Ukládat pouze testy, které selhaly")
	args = parser.parse_args()

	grammar = DEFAULT_GRAMMAR
	if args.grammar:
		with open(args.grammar, "r", encoding="utf-8") as f:
			grammar = json.load(f)

	out_path = Path(args.out_dir)
	out_path.mkdir(parents=True, exist_ok=True)

	rate = 0.05 if args.mutations else 0.0
	bin_exists = Path(args.bin).is_file()

	print(f"🚀 Fuzzer startuje: {args.count} vzorků (výstup: {out_path}/)...")

	saved_count = 0
	for i in range(1, args.count + 1):
		fuzzer = GrammarFuzzer(grammar, max_depth=12, mutation_rate=rate)
		code = fuzzer.generate()
		expect_error = fuzzer.has_mutated

		tag = "syntax_err" if expect_error else "valid"
		file_name = f"test_{i:04d}_{tag}.ifj"
		file_path = out_path / file_name

		if bin_exists:
			ok, msg = run_test(args.bin, code, expect_error)
			if not ok or not args.save_failed_only:
				header = f"// Test: {file_name}\n// Expect error: {expect_error}\n// Result: {msg}\n\n"
				file_path.write_text(header + code, encoding="utf-8")
				saved_count += 1

			if not ok:
				print(f"❌ Selhal test #{i} ({file_name}): {msg}")
		else:
			# Režim bez binárky: uložení vzorků pro inspekci
			header = f"// Test: {file_name}\n// Expect error: {expect_error}\n\n"
			file_path.write_text(header + code, encoding="utf-8")
			saved_count += 1

	print(f"✨ Hotovo. Uloženo {saved_count} souborů do adresáře '{out_path}'.")


if __name__ == "__main__":
	main()