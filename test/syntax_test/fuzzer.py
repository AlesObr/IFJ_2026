#!/usr/bin/env python3
"""
Fuzzer pro IFJ26 pracujici se sadami souboru (.scala, .in, .out).
Generuje validni varianty i syntakticke chyby (navratovy kod 2) pro parser.
"""

import argparse
import random
import re
import shutil
from pathlib import Path


class IFJCodeMutator:
    # Rozsireny seznam rezervovanych slov dle specifikace IFJ26 (zaklad + rozsireni)
    RESERVED = {
        "_", "ifj26", "def", "val", "var", "if", "then", "else", "while", "do",
        "return", "import", "Int", "Double", "String", "Unit", "Boolean",
        "case", "class", "extension", "for", "List", "match", "type",
        "to", "until", "by", "main", "true", "false",
        # Vestavene funkce a cleny (zaklad + rozsireni)
        "print", "readLine", "readInt", "readDouble", "readBoolean",
        "readIntMaybe", "readDoubleMaybe", "toStr", "toInt", "strLength",
        "subStr", "ord", "chr", "toString", "floor", "length",
        "drop", "take", "slice", "head", "tail", "range",
        "filter", "takeWhile", "dropWhile", "toList"
    }

    def __init__(self, code: str):
        self.code = code
        self.lines = code.splitlines(keepends=True)

    @staticmethod
    def _strip_comments_and_strings(code: str) -> str:
        """
        Nahradi obsah retezcu a komentaru mezerami (zachova delku a radkovani),
        aby regularni vyrazy nemutovaly obsah uvnitr retezcu a komentaru.
        """
        pattern = re.compile(
            r'("""[\s\S]*?"""|"(?:\\.|[^"\\])*"|//[^\n]*|/\*[\s\S]*?\*/)'
        )
        return pattern.sub(
            lambda m: "".join("\n" if c == "\n" else " " for c in m.group(0)),
            code
        )

    # --- Validni mutace (zachovavaji syntaktickou i semantickou spravnost) ---

    def mutate_identifiers(self) -> str:
        """Prejmenuje bezpecne identifikatory promennych ci funkci (mimo retezce a komentare)."""
        clean_code = self._strip_comments_and_strings(self.code)

        # Najdeme vsechny identifikatory, ktere nejsou soucasti ciselnych literalu (napr. 1e10)
        tokens = re.findall(r"(?<![0-9.])\b[a-zA-Z_][a-zA-Z0-9_]*\b", clean_code)
        candidates = list({
            t for t in tokens
            if t not in self.RESERVED and not t.startswith("@")
        })
        if not candidates:
            return self.code

        target = random.choice(candidates)
        replacement = f"{target}_fuzz{random.randint(10, 99)}"

        # Nahradime pouze vyskyty v cistem kodu (mimo retezce a komentare)
        result = []
        last_idx = 0
        for match in re.finditer(rf"(?<![0-9.])\b{re.escape(target)}\b", clean_code):
            start, end = match.span()
            result.append(self.code[last_idx:start])
            result.append(replacement)
            last_idx = end
        result.append(self.code[last_idx:])
        return "".join(result)

    def mutate_formatting(self) -> str:
        """Vklada povolene ASCII komentare a prazdne radky mimo viceradkove retezce."""
        res = []
        in_multiline_str = False
        in_block_comment = False

        for line in self.lines:
            # Jednoducha detekce viceradkovych retezcu a blokovych komentaru
            if '"""' in line:
                if line.count('"""') % 2 == 1:
                    in_multiline_str = not in_multiline_str
                res.append(line)
                continue

            if "/*" in line or "*/" in line:
                if "/*" in line and "*/" not in line:
                    in_block_comment = True
                elif "*/" in line:
                    in_block_comment = False
                res.append(line)
                continue

            if not in_multiline_str and not in_block_comment:
                if random.random() < 0.15:
                    # Pouze ciste ASCII znaky! (Zadne hacky a carky)
                    res.append("// Fuzz vlozeny komentar\n")
                if random.random() < 0.10:
                    res.append("\n")
                stripped = line.strip()
                if stripped and not stripped.startswith("//") and "//" not in line and random.random() < 0.15:
                    line = line.rstrip("\r\n") + " /* inline */\n"

            res.append(line)
        return "".join(res)

    def generate_valid(self) -> str:
        res = self.code
        for fn_name in ["mutate_identifiers", "mutate_formatting"]:
            if random.random() < 0.7:
                res = getattr(IFJCodeMutator(res), fn_name)()
        return res

    # --- Syntakticke chyby (navratovy kod 2) ---

    def err_indentation(self) -> str:
        """Narusi striktni odsazeni bloku mezerami."""
        res = []
        mutated = False
        for line in self.lines:
            match = re.match(r"^( +)(\S.*)$", line)
            if match and not mutated and not line.strip().startswith("//") and random.random() < 0.35:
                spaces, rest = match.groups()
                # Zvyseni odsazeni uprostred bloku bez hlavicky zarucene rozbije strukturu bloku (chyba 2)
                new_len = len(spaces) + random.choice([1, 3, 5])
                res.append(" " * new_len + rest + "\n")
                mutated = True
            else:
                res.append(line)
        return "".join(res) if mutated else self.code + "\n   val x_err = 1\n"

    def err_unbalanced_delimiters(self) -> str:
        """Smaze nebo vlozi prebytecnou kulatou zavorku mimo retezce a komentare."""
        clean_code = self._strip_comments_and_strings(self.code)
        paren_positions = [m.start() for m in re.finditer(r"[()]", clean_code)]

        if paren_positions and random.random() < 0.6:
            pos = random.choice(paren_positions)
            return self.code[:pos] + self.code[pos + 1:]

        # Vlozeni zavorky na konec nektereho radku s kodem
        for i, line in enumerate(self.lines):
            stripped = line.strip()
            if stripped and not stripped.startswith("//") and not stripped.startswith("import"):
                line_no_nl = line.rstrip("\r\n")
                return "".join(self.lines[:i] + [line_no_nl + " )\n"] + self.lines[i + 1:])
        return self.code + " (\n"

    def err_drop_keyword(self) -> str:
        """Odstrani povinne klicove slovo nebo samostatne prirazovaci '='."""
        clean_code = self._strip_comments_and_strings(self.code)
        # Pozor: '=' nesmi byt soucasti ==, !=, <=, >=, =>
        kws = [r"\bthen\b", r"\bdo\b", r"\bdef\b", r"(?<![=!<>])=(?![=>])"]
        random.shuffle(kws)

        for kw in kws:
            match = re.search(kw, clean_code)
            if match:
                start, end = match.span()
                return self.code[:start] + " " + self.code[end:]
        return self.code + "\ndef\n"

    def err_operator_line_start(self) -> str:
        """Zacne logicky radek binarnim operatorem (zakazano dle sekce 1.2)."""
        # Vynechame '+' a '-', protoze by mohly byt validnim unarnim operatorem!
        ops = ["*", "/", "==", "!=", "<="]
        for i, line in enumerate(self.lines):
            match = re.match(r"^(\s*)(\S.*)$", line)
            if match:
                indent, rest = match.groups()
                if not rest.startswith("//") and not rest.startswith("/*") and not rest.startswith("import") and not rest.startswith("@"):
                    return "".join(
                        self.lines[:i] + [f"{indent}{random.choice(ops)} {rest}\n"] + self.lines[i + 1:]
                    )
        return "* " + self.code

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
    parser.add_argument("--seeds-dir", type=str, default=".", help="Adresar se soubory .scala, .in, .out")
    parser.add_argument("--out-dir", type=str, default="generated_tests", help="Vystupni adresar")
    parser.add_argument("--valid-count", type=int, default=15, help="Pocet validnich testu")
    parser.add_argument("--invalid-count", type=int, default=30, help="Pocet syntaktickych chyb")
    args = parser.parse_args()

    seeds_dir = Path(args.seeds_dir)
    # Vyfiltrujeme pripadny knihovni soubor ifj26.scala, aby nebyl mutovan jako test
    scala_files = [f for f in seeds_dir.glob("*.scala") if f.name != "ifj26.scala"]
    if not scala_files:
        print(f"Chyba: Zadne .scala soubory v '{seeds_dir}'.")
        return

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    print(f"Nalezeno {len(scala_files)} seedu: {[f.name for f in scala_files]}")

    # 1. Generovani validnich vzorku
    for i in range(1, args.valid_count + 1):
        src = random.choice(scala_files)
        mutator = IFJCodeMutator(src.read_text(encoding="utf-8"))
        code = mutator.generate_valid()

        base_name = f"test_valid_{i:03d}_{src.stem}"
        (out_dir / f"{base_name}.scala").write_text(code, encoding="utf-8")

        # Zkopirovani prislusneho .in a .out souboru, pokud existuji
        for ext in [".in", ".out"]:
            ref_file = src.with_suffix(ext)
            target_file = out_dir / f"{base_name}{ext}"
            if ref_file.exists():
                shutil.copy(ref_file, target_file)
            else:
                target_file.touch()

    # 2. Generovani syntakticky chybnych vzorku
    for i in range(1, args.invalid_count + 1):
        src = random.choice(scala_files)
        mutator = IFJCodeMutator(src.read_text(encoding="utf-8"))
        code, err_tag = mutator.generate_error()

        base_name = f"test_error_{i:03d}_{err_tag}_{src.stem}"
        (out_dir / f"{base_name}.scala").write_text(code, encoding="utf-8")
        (out_dir / f"{base_name}.in").touch()

    print(f"Hotovo. Vygenerovano {args.valid_count + args.invalid_count} testu v '{out_dir}/'.")


if __name__ == "__main__":
    main()