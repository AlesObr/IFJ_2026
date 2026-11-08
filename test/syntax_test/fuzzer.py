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
        result = list(code)
        for start, end in IFJCodeMutator._literal_spans(code):
            for pos in range(start, end):
                if result[pos] != "\n":
                    result[pos] = " "
        return "".join(result)

    @staticmethod
    def _literal_spans(code: str) -> list[tuple[int, int]]:
        """Vrati rozsahy (start, end) vsech retezcu a komentaru ve zdrojovem kodu."""
        spans = []
        length = len(code)
        i = 0

        while i < length:
            start = i
            if code.startswith('"""', i):
                end = code.find('"""', i + 3)
                i = length if end < 0 else end + 3
            elif code[i] == '"':
                i += 1
                while i < length and code[i] not in '"\n':
                    i += 2 if code[i] == "\\" else 1
                i = min(i + 1, length)
            elif code.startswith("//", i):
                end = code.find("\n", i)
                i = length if end < 0 else end
            elif code.startswith("/*", i):
                # Blokove komentare se mohou vnorovat
                depth = 1
                i += 2
                while i < length and depth > 0:
                    if code.startswith("/*", i):
                        depth += 1
                        i += 2
                    elif code.startswith("*/", i):
                        depth -= 1
                        i += 2
                    else:
                        i += 1
            else:
                i += 1
                continue
            spans.append((start, i))
        return spans

    def _code_lines(self) -> list[tuple[int, int | None, int, int]]:
        """
        Pro kazdy fyzicky radek vrati (index, odsazeni, hloubka zavorek na zacatku,
        pozice za poslednim znakem kodu). Odsazeni je None, pokud radek nezacina kodem
        (prazdny radek, komentar, retezec nebo pokracovani viceradkoveho retezce ci komentare).
        """
        clean_code = self._strip_comments_and_strings(self.code)
        result = []
        offset = 0
        depth = 0
        for index, line in enumerate(self.lines):
            clean_line = clean_code[offset:offset + len(line)]
            offset += len(line)
            stripped = clean_line.lstrip(" ")
            indent = len(clean_line) - len(stripped)
            starts_with_code = (
                stripped.strip() != ""
                and line[:indent] == " " * indent
                and line[indent] == clean_line[indent]
            )
            code_end = len(clean_line.rstrip())
            result.append((index, indent if starts_with_code else None, depth, code_end))
            depth += clean_line.count("(") - clean_line.count(")")
        return result

    # --- Validni mutace (zachovavaji syntaktickou i semantickou spravnost) ---

    def mutate_identifiers(self) -> str:
        """Prejmenuje bezpecne identifikatory promennych ci funkci (mimo retezce a komentare)."""
        clean_code = self._strip_comments_and_strings(self.code)

        # Najdeme vsechny identifikatory, ktere nejsou soucasti ciselnych literalu (napr. 1e10)
        tokens = re.findall(r"(?<![0-9.])\b[a-zA-Z_][a-zA-Z0-9_]*\b", clean_code)
        # Serazeno: poradi mnoziny retezcu se meni mezi behy a rozbilo by --seed
        candidates = sorted({
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
        spans = self._literal_spans(self.code)

        def inside_literal(pos: int) -> bool:
            return any(start < pos < end for start, end in spans)

        offset = 0
        for line in self.lines:
            line_start = offset
            offset += len(line)
            newline_pos = line_start + len(line.rstrip("\r\n"))

            # Do viceradkoveho retezce ci komentare se nesmi nic vlozit
            if not inside_literal(line_start):
                if random.random() < 0.15:
                    # Pouze ciste ASCII znaky! (Zadne hacky a carky)
                    res.append("// Fuzz vlozeny komentar\n")
                if random.random() < 0.10:
                    res.append("\n")

            if line.strip() and not inside_literal(newline_pos) and random.random() < 0.15:
                line = line.rstrip("\r\n") + " /* inline */\n"

            res.append(line)
        return "".join(res)

    def generate_valid(self) -> tuple[str, bool]:
        """Vrati kod a priznak, zda mutace zachovala AST (prejmenovani ho meni)."""
        res = self.code
        same_ast = True
        for fn_name in ["mutate_identifiers", "mutate_formatting"]:
            if random.random() < 0.7:
                res = getattr(IFJCodeMutator(res), fn_name)()
                same_ast = same_ast and fn_name != "mutate_identifiers"
        return res, same_ast

    # --- Syntakticke chyby (navratovy kod 2) ---

    def err_indentation(self) -> str:
        """
        Narusi striktni odsazeni bloku mezerami.

        Vybira jen radky se stejnym odsazenim jako predchozi prikaz. Hlubsi odsazeni
        prvniho prikazu bloku, pokracovani v zavorkach nebo obsahu viceradkoveho
        retezce by program nerozbilo a vznikl by falesne chybny test.
        """
        candidates = []
        previous_indent = None
        for index, indent, depth, _ in self._code_lines():
            if indent is None or depth > 0:
                continue
            if indent > 0 and indent == previous_indent:
                candidates.append((index, indent))
            previous_indent = indent

        if not candidates:
            # Hlubsi odsazeni nez posledni prikaz, po kterem nemuze zacit novy blok
            last_indent = previous_indent if previous_indent is not None else 0
            return self.code + "\n" + " " * (last_indent + 1) + "val x_err = 1\n"

        index, indent = random.choice(candidates)
        res = list(self.lines)
        res[index] = " " * random.choice([1, 3, 5]) + res[index]
        return "".join(res)

    def err_unbalanced_delimiters(self) -> str:
        """Smaze nebo vlozi prebytecnou kulatou zavorku mimo retezce a komentare."""
        clean_code = self._strip_comments_and_strings(self.code)
        paren_positions = [m.start() for m in re.finditer(r"[()]", clean_code)]

        if paren_positions and random.random() < 0.6:
            pos = random.choice(paren_positions)
            return self.code[:pos] + self.code[pos + 1:]

        # Vlozeni zavorky na konec nektereho radku s kodem
        for i, indent, _, code_end in self._code_lines():
            line = self.lines[i]
            if indent is not None and not line.strip().startswith("import"):
                # Vlozit hned za kod, jinak by zavorka mohla skoncit v komentari na konci radku
                mutated = line[:code_end] + " )" + line[code_end:]
                if not mutated.endswith("\n"):
                    mutated += "\n"
                return "".join(self.lines[:i] + [mutated] + self.lines[i + 1:])
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
        for i, indent, depth, _ in self._code_lines():
            if indent is None or depth > 0:
                continue
            line = self.lines[i]
            rest = line[indent:].rstrip("\r\n")
            if not rest.startswith("import") and not rest.startswith("@"):
                return "".join(
                    self.lines[:i] + [f"{' ' * indent}{random.choice(ops)} {rest}\n"] + self.lines[i + 1:]
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
    parser.add_argument("--seed", type=int, default=None, help="Seed generatoru pro reprodukovatelny beh")
    parser.add_argument("--clean", action="store_true", help="Pred generovanim smazat drive vygenerovane test_valid_* a test_error_* ve vystupnim adresari")
    args = parser.parse_args()

    if args.seed is not None:
        random.seed(args.seed)

    seeds_dir = Path(args.seeds_dir)
    # Vyfiltrujeme pripadny knihovni soubor ifj26.scala, aby nebyl mutovan jako test
    scala_files = sorted(f for f in seeds_dir.glob("*.scala") if f.name != "ifj26.scala")
    if not scala_files:
        print(f"Chyba: Zadne .scala soubory v '{seeds_dir}'.")
        return

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    if args.clean:
        for pattern in ("test_valid_*", "test_error_*"):
            for old_file in out_dir.glob(pattern):
                old_file.unlink()

    print(f"Nalezeno {len(scala_files)} seedu: {[f.name for f in scala_files]}")

    # 1. Generovani validnich vzorku
    for i in range(1, args.valid_count + 1):
        src = random.choice(scala_files)
        mutator = IFJCodeMutator(src.read_text(encoding="utf-8"))
        code, same_ast = mutator.generate_valid()

        base_name = f"test_valid_{i:03d}_{src.stem}"
        (out_dir / f"{base_name}.scala").write_text(code, encoding="utf-8")

        # Mutace zachovavajici AST se daji porovnat s referencnim stromem seedu
        ast_file = src.with_suffix(".ast")
        ast_target = out_dir / f"{base_name}.ast"
        if same_ast and ast_file.exists():
            shutil.copy(ast_file, ast_target)
        else:
            # Reference z predchozího behu by k novemu kodu nepatrila
            ast_target.unlink(missing_ok=True)

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