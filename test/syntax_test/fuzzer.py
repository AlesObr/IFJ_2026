#!/usr/bin/env python3
"""
Fuzzer pro IFJ26 pracující se sadami souborů (.scala, .in, .out).
Generuje validní varianty i syntaktické chyby pro parser.
Soubor byl vygenerován nástrojem generativní umělé inteligence (Gemini 3.8 flash)
"""

import argparse
import random
import re
import shutil
from pathlib import Path


class IFJCodeMutator:
    def __init__(self, code: str):
        self.code = code
        self.lines = code.splitlines(keepends=True)

    # --- Validní mutace (zachovávají syntaktickou správnost) ---

    def mutate_identifiers(self) -> str:
        """Přejmenuje bezpečné identifikátory proměnných či funkcí."""
        reserved = {
            "def", "val", "var", "if", "then", "else", "while", "do",
            "return", "import", "Int", "Double", "String", "Unit", "Boolean",
            "main", "true", "false", "print", "readLine", "readInt", "readDouble",
            "toStr", "toInt", "strLength", "subStr", "ord", "chr"
        }
        tokens = re.findall(r"\b[a-zA-Z_][a-zA-Z0-9_]*\b", self.code)
        candidates = list({t for t in tokens if t not in reserved and not t.startswith("@")})
        if not candidates:
            return self.code

        target = random.choice(candidates)
        replacement = f"{target}_fuzz{random.randint(10, 99)}"
        return re.sub(rf"\b{re.escape(target)}\b", replacement, self.code)

    def mutate_formatting(self) -> str:
        """Vkládá povolené komentáře a prázdné řádky."""
        res = []
        for line in self.lines:
            if random.random() < 0.15:
                res.append("// Fuzz vložený komentář\n")
            if random.random() < 0.1:
                res.append("\n")
            if line.strip() and not line.strip().startswith("//") and random.random() < 0.15:
                line = line.rstrip("\r\n") + " /* inline */\n"
            res.append(line)
        return "".join(res)

    def generate_valid(self) -> str:
        res = self.code
        for fn in [self.mutate_identifiers, self.mutate_formatting]:
            if random.random() < 0.7:
                res = getattr(IFJCodeMutator(res), fn.__name__)()
        return res

    # --- Syntaktické a lexikální chyby (pro testování chybových stavů parseru) ---

    def err_indentation(self) -> str:
        """Naruší striktní odsazení bloku mezerami."""
        res = []
        mutated = False
        for line in self.lines:
            match = re.match(r"^( +)(\S.*)$", line)
            if match and not mutated and random.random() < 0.35:
                spaces, rest = match.groups()
                # Změna počtu mezer na nevalidní úroveň
                new_len = max(0, len(spaces) + random.choice([-3, -1, 1, 3]))
                res.append(" " * new_len + rest + "\n")
                mutated = True
            else:
                res.append(line)
        return "".join(res) if mutated else self.code + "\n   x = 1\n"

    def err_unbalanced_delimiters(self) -> str:
        """Smaže nebo vloží přebytečnou závorku."""
        tokens = ["(", ")", "{", "}"]
        t = random.choice(tokens)
        if random.random() < 0.5 and t in self.code:
            pos = self.code.rfind(t)
            return self.code[:pos] + self.code[pos + 1:]
        idx = random.randint(0, len(self.code) - 1)
        return self.code[:idx] + t + self.code[idx:]

    def err_drop_keyword(self) -> str:
        """Odstraní povinné klíčové slovo (then, do, def, =)."""
        kws = [r"\bthen\b", r"\bdo\b", r"\bdef\b", r"="]
        kw = random.choice(kws)
        if re.search(kw, self.code):
            return re.sub(kw, "", self.code, count=1)
        return self.code + " def"

    def err_operator_line_start(self) -> str:
        """Začne logický řádek operátorem (zakázáno dle specifikace)."""
        ops = ["+", "*", "-", "/", "=="]
        for i, line in enumerate(self.lines):
            match = re.match(r"^(\s*)(\S.*)$", line)
            if match and not line.strip().startswith("//") and not line.strip().startswith("import"):
                indent, rest = match.groups()
                return "".join(self.lines[:i] + [f"{indent}{random.choice(ops)} {rest}\n"] + self.lines[i+1:])
        return self.code

    def generate_error(self) -> tuple[str, str]:
        errors = [
            (self.err_indentation, "indent_err"),
            (self.err_unbalanced_delimiters, "delim_err"),
            (self.err_drop_keyword, "missing_kw"),
            (self.err_operator_line_start, "op_start_err"),
        ]
        fn, name = random.choice(errors)
        return fn(), name


def main():
    parser = argparse.ArgumentParser(description="IFJ26 Seed-based Fuzzer")
    parser.add_argument("--seeds-dir", type=str, default=".", help="Adresář se soubory .scala a .in")
    parser.add_argument("--out-dir", type=str, default="generated_tests", help="Výstupní adresář")
    parser.add_argument("--valid-count", type=int, default=15, help="Počet validních testů")
    parser.add_argument("--invalid-count", type=int, default=30, help="Počet syntaktických chyb")
    args = parser.parse_args()

    seeds_dir = Path(args.seeds_dir)
    scala_files = list(seeds_dir.glob("*.scala"))
    if not scala_files:
        print(f"❌ Žádné .scala soubory v '{seeds_dir}'.")
        return

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    print(f" Nalezeno {len(scala_files)} seedů: {[f.name for f in scala_files]}")

    # 1. Generování validních vzorků
    for i in range(1, args.valid_count + 1):
        src = random.choice(scala_files)
        mutator = IFJCodeMutator(src.read_text(encoding="utf-8"))
        code = mutator.generate_valid()

        base_name = f"test_valid_{i:03d}_{src.stem}"
        (out_dir / f"{base_name}.scala").write_text(code, encoding="utf-8")

        # Zkopírování příslušného .in souboru, pokud existuje
        in_file = src.with_suffix(".in")
        if in_file.exists():
            shutil.copy(in_file, out_dir / f"{base_name}.in")
        else:
            (out_dir / f"{base_name}.in").touch()

    # 2. Generování syntakticky chybných vzorků
    for i in range(1, args.invalid_count + 1):
        src = random.choice(scala_files)
        mutator = IFJCodeMutator(src.read_text(encoding="utf-8"))
        code, err_tag = mutator.generate_error()

        base_name = f"test_error_{i:03d}_{err_tag}_{src.stem}"
        (out_dir / f"{base_name}.scala").write_text(code, encoding="utf-8")
        
        # Pro chybné testy stačí prázdný .in soubor
        (out_dir / f"{base_name}.in").touch()

    print(f" Hotovo. Vygenerováno {args.valid_count + args.invalid_count} testů v '{out_dir}/'.")


if __name__ == "__main__":
    main()