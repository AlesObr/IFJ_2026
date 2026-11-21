#!/usr/bin/env python3
"""
Fuzzer pro IFJ26 pracujici se sadami souboru (.scala, .in, .out).
Generuje validni varianty i syntakticke chyby (navratovy kod 2) pro parser.
"""

import argparse
import random
import re
import shutil
import subprocess
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
        self._clean_cache: list[str] | None = None
        self._code_lines_cache: list[tuple[int, int | None, int, int]] | None = None

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
        """Returns (start, end) ranges of all strings and comments in the source."""
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
                # Block comments can be nested
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

    def _layout_code(self) -> str:
        """
        Code for layout analysis: comments blanked out, string contents replaced by '0' (the
        opening quote kept). A string thus still counts as an operand, so a line ending with
        a string does not look like a line ending with an operator, and code_end includes it.
        """
        result = list(self.code)
        for start, end in self._literal_spans(self.code):
            is_string = self.code[start] == '"'
            for pos in range(start, end):
                if result[pos] != "\n":
                    result[pos] = "0" if is_string and pos > start else ('"' if is_string else " ")
        return "".join(result)

    def _code_lines(self) -> list[tuple[int, int | None, int, int]]:
        """
        Returns (index, indentation, paren depth at line start, position after the last
        code character) for each physical line. Indentation is None if the line does not
        start with code (blank line, comment, string or continuation of a multiline
        string or comment).
        """
        if self._code_lines_cache is not None:
            return self._code_lines_cache
        clean_code = self._layout_code()
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
        self._code_lines_cache = result
        return result

    # Binary operators that continue an expression on the next line when they end a line
    # (the prolog "import ifj26.*" ends with '*' too, it is excluded by the negative lookahead)
    _TRAILING_OPERATOR = re.compile(r"^(?!\s*import\b).*(?:[-+*/<>]|[=!<>]=)$")
    # Binary operator surrounded by spaces, preceded by an operand (so not a unary minus)
    _BINARY_OPERATOR = re.compile(r"(?<=[\w)\"]) +(==|!=|<=|>=|[-+*/<>]) +")

    def _clean_line(self, index: int) -> str:
        """Line with strings and comments blanked out (cached, mutations call it per line)."""
        if self._clean_cache is None:
            clean_code = self._layout_code()
            self._clean_cache = []
            offset = 0
            for line in self.lines:
                self._clean_cache.append(clean_code[offset:offset + len(line)])
                offset += len(line)
        return self._clean_cache[index]

    def _ends_with_operator(self) -> set[int]:
        """
        Indexes of lines whose code ends with a binary operator. A blank or comment line must
        not follow them: Scala ends the continued expression there (the oracle would reject it).
        """
        return {index for index, _, _, code_end in self._code_lines()
                if self._TRAILING_OPERATOR.search(self._clean_line(index)[:code_end].rstrip())}

    def _continuation_lines(self) -> set[int]:
        """Indexes of lines that continue an expression (previous code line ends with an operator)."""
        result = set()
        previous_code = None
        clean_lines = [self._clean_line(i) for i in range(len(self.lines))]
        for index, indent, depth, code_end in self._code_lines():
            if indent is None and code_end == 0:
                continue
            if previous_code is not None and (depth > 0 or self._TRAILING_OPERATOR.search(previous_code)):
                result.add(index)
            previous_code = clean_lines[index][:code_end].rstrip() if index < len(clean_lines) else ""
        return result

    def _statement_lines(self) -> list[tuple[int, int]]:
        """(index, indent) of lines starting a statement: code at line start, outside parens, not a continuation."""
        continuation = self._continuation_lines()
        return [(index, indent) for index, indent, depth, _ in self._code_lines()
                if indent is not None and depth == 0 and index not in continuation]

    def _block_parents(self) -> dict[int, int]:
        """Statement line index -> indentation of the enclosing block's header (-1 on top level)."""
        parents = {}
        stack = [0]
        for index, indent in self._statement_lines():
            while len(stack) > 1 and indent < stack[-1]:
                stack.pop()
            if indent > stack[-1]:
                stack.append(indent)
            parents[index] = stack[-2] if len(stack) > 1 else -1
        return parents

    # --- Validni mutace (zachovavaji syntaktickou i semantickou spravnost) ---

    def mutate_identifiers(self) -> str:
        """Prejmenuje bezpecne identifikatory promennych ci funkci (mimo retezce a komentare)."""
        clean_code = self._strip_comments_and_strings(self.code)

        # Najdeme vsechny identifikatory, ktere nejsou soucasti ciselnych literalu (napr. 1e10)
        tokens = re.findall(r"(?<![0-9.])\b[a-zA-Z_][a-zA-Z0-9_]*\b", clean_code)
        # Sorted: the order of a string set changes between runs and would break --seed
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

        ends_with_operator = self._ends_with_operator()
        after_operator = False
        offset = 0
        for index, line in enumerate(self.lines):
            line_start = offset
            offset += len(line)
            newline_pos = line_start + len(line.rstrip("\r\n"))

            # Nothing may be inserted into a multiline string or comment, nor into a continued expression
            if not inside_literal(line_start) and not after_operator:
                if random.random() < 0.15:
                    # Pouze ciste ASCII znaky! (Zadne hacky a carky)
                    res.append("// Fuzz vlozeny komentar\n")
                if random.random() < 0.10:
                    res.append("\n")

            if line.strip() and not inside_literal(newline_pos) and random.random() < 0.15:
                line = line.rstrip("\r\n") + " /* inline */\n"

            if line.strip():
                after_operator = index in ends_with_operator
            res.append(line)
        return "".join(res)

    def mutate_comments_between_tokens(self) -> str:
        """Vlozi blokove komentare (i vnorene) do mezer mezi tokeny."""
        spans = self._literal_spans(self.code)
        comments = ["/* c */", "/**/", "/* a /* b */ c */", "/* ) ( , = */", "/* \" */"]
        out = []
        offset = 0
        for index, line in enumerate(self.lines):
            stripped = line.lstrip(" ")
            if stripped.startswith(("import", "@")):
                out.append(line)
                offset += len(line)
                continue
            chars = list(line)
            indent = len(line) - len(stripped)
            for pos in range(len(line) - 1, indent, -1):
                absolute = offset + pos
                if chars[pos] == " " and random.random() < 0.08 and \
                        not any(start <= absolute < end for start, end in spans):
                    chars[pos] = f" {random.choice(comments)} "
            out.append("".join(chars))
            offset += len(line)
        return "".join(out)

    def mutate_wrap_literals(self) -> str:
        """Obali ciselne a retezcove literaly redundantnimi zavorkami (AST se nemeni)."""
        clean_code = self._strip_comments_and_strings(self.code)
        spans = [span for span in self._literal_spans(self.code) if self.code[span[0]] == '"'
                 and not self.code.startswith('"""', span[0])]
        numbers = [m.span() for m in re.finditer(r"(?<![\w.])\d[\d_]*(?:\.\d[\d_]*)?(?:[eE][+-]?\d+)?[dD]?(?![\w.])", clean_code)]
        targets = sorted(t for t in spans + numbers if random.random() < 0.3)
        result = self.code
        for start, end in reversed(targets):
            result = result[:start] + "(" + result[start:end] + ")" + result[end:]
        return result

    def mutate_line_breaks(self) -> str:
        """
        Zalomi radek uvnitr zavorek za '(', ',' nebo binarnim operatorem (LF v zavorkach je bily
        znak). Mimo zavorky by slo dle zadani 1.2 o chybu, viz err_trailing_operator. Dalsi radek
        musi byt odsazen vic nez zacatek logickeho radku.
        """
        result = list(self.lines)
        continuation = self._continuation_lines()
        for index, indent, depth, code_end in reversed(self._code_lines()):
            if indent is None or index in continuation or random.random() > 0.25:
                continue
            line = self.lines[index]
            clean = self._clean_line(index)[:code_end]
            if clean.lstrip().startswith(("import", "@", "def")):
                continue
            positions = []
            for match in self._BINARY_OPERATOR.finditer(clean):
                before = clean[:match.start()]
                if before.count("(") > before.count(")") and clean[match.end():].strip():
                    positions.append(match.end())
            for match in re.finditer(r"[(,] *", clean):
                before = clean[:match.start() + 1]
                if before.count("(") > before.count(")") and clean[match.end():].strip():
                    positions.append(match.end())
            if not positions:
                continue
            pos = random.choice(positions)
            new_indent = " " * (indent + random.randint(1, 8))
            result[index] = line[:pos].rstrip(" ") + "\n" + new_indent + line[pos:]
        return "".join(result)

    def mutate_semicolons(self) -> str:
        """Spoji dva jednoduche prikazy stejne urovne na jeden radek pres ';'."""
        statements = dict(self._statement_lines())
        info = {index: (indent, depth, code_end) for index, indent, depth, code_end in self._code_lines()}
        result = list(self.lines)
        index = 0
        joined = set()
        while index < len(self.lines) - 2:
            nxt, after = index + 1, index + 2
            if index in statements and nxt in statements and statements[index] == statements[nxt] \
                    and index not in joined:
                clean = self._clean_line(index)[:info[index][2]].rstrip()
                clean_next = self._clean_line(nxt)[:info[nxt][2]].rstrip()
                next_header = after in statements and statements[after] > statements[nxt]
                no_comment = len(self.lines[index].rstrip()) == info[index][2] and \
                    len(self.lines[nxt].rstrip()) == info[nxt][2]
                simple = not re.search(r"(\bthen|\bdo|\belse|=|[-+*/<>,(])$", clean) and \
                    not re.search(r"(\bthen|\bdo|\belse|=)$", clean_next) and \
                    not clean.lstrip().startswith(("@", "import", "def", "else")) and \
                    not clean_next.lstrip().startswith(("else", "def", "@"))
                ends_balanced = clean.count("(") == clean.count(")") and clean_next.count("(") == clean_next.count(")")
                if simple and no_comment and not next_header and ends_balanced and random.random() < 0.3:
                    result[index] = self.lines[index].rstrip("\r\n") + random.choice([";", "; ", " ;  "]) + \
                        self.lines[nxt].lstrip(" ")
                    result[nxt] = ""
                    joined.add(nxt)
                    index += 2
                    continue
            index += 1
        return "".join(result)

    def mutate_whitespace(self) -> str:
        """Koncove mezery a radky obsahujici jen mezery (nesmi vytvorit INDENT/OUTDENT)."""
        spans = self._literal_spans(self.code)
        ends_with_operator = self._ends_with_operator()
        out = []
        offset = 0
        for index, line in enumerate(self.lines):
            end_of_line = offset + len(line.rstrip("\r\n"))
            offset += len(line)
            if not any(start < end_of_line < end for start, end in spans):
                if line.endswith("\n") and random.random() < 0.1:
                    line = line[:-1] + " " * random.randint(1, 4) + "\n"
                if random.random() < 0.05 and index not in ends_with_operator:
                    line = line + " " * random.randint(1, 12) + "\n"
            out.append(line)
        return "".join(out)

    VALID_MUTATIONS = {
        "mutate_identifiers": 0.6,
        "mutate_formatting": 0.6,
        "mutate_comments_between_tokens": 0.3,
        "mutate_wrap_literals": 0.3,
        "mutate_line_breaks": 0.3,
        "mutate_semicolons": 0.3,
        "mutate_whitespace": 0.3,
    }

    def generate_valid(self) -> tuple[str, bool]:
        """Returns the code and whether the mutation preserved the AST (renaming changes it)."""
        res = self.code
        same_ast = True
        for fn_name, probability in self.VALID_MUTATIONS.items():
            if random.random() < probability:
                res = getattr(IFJCodeMutator(res), fn_name)()
                same_ast = same_ast and fn_name != "mutate_identifiers"
        return res, same_ast

    # --- Syntakticke chyby (navratovy kod 2) ---

    def err_indentation(self) -> str:
        """
        Breaks the strict indentation of a block.

        Only picks lines indented the same as the previous statement. Indenting the first
        statement of a block, a continuation inside parentheses or the content of a
        multiline string would not break the program and would create a mislabelled test.
        """
        candidates = []
        previous_indent = None
        # A continuation of an expression may be indented arbitrarily, so it is skipped
        for index, indent in self._statement_lines():
            if indent > 0 and indent == previous_indent:
                candidates.append((index, indent))
            previous_indent = indent

        if not candidates:
            # Deeper than the last statement, after which no new block can start
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
                # Insert right after the code, otherwise the paren could end up in a trailing comment
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

    def err_dedent_between(self) -> str:
        """Odsadi prikaz na uroven mezi nadrazenym blokem a blokem, ve kterem lezi (neexistujici uroven)."""
        statement_lines = self._statement_lines()
        statements = dict(statement_lines)
        # The first statement of a block may be indented arbitrarily, so only later ones qualify
        not_first = {index for (_, previous), (index, indent) in zip(statement_lines, statement_lines[1:])
                     if previous >= indent}
        candidates = [(index, statements[index], parent) for index, parent in self._block_parents().items()
                      if index in not_first and parent >= 0 and statements[index] - parent >= 2]
        if not candidates:
            return self.err_indentation()
        index, indent, parent = random.choice(candidates)
        res = list(self.lines)
        res[index] = " " * random.randint(parent + 1, indent - 1) + res[index].lstrip(" ")
        return "".join(res)

    def err_truncate(self) -> str:
        """Usekne program uvnitr zavorek nebo hned za binarnim operatorem."""
        clean_code = self._strip_comments_and_strings(self.code)
        inside, after_operator = [], []
        depth = 0
        for pos, char in enumerate(clean_code):
            if char == "(":
                depth += 1
                inside.append(pos + 1)
            elif char == ")":
                depth -= 1
            elif char == "," and depth > 0:
                inside.append(pos + 1)
        for match in self._BINARY_OPERATOR.finditer(clean_code):
            # End of the operator itself (match.end() could lie after a blanked string)
            after_operator.append(match.end(1))
        positions = inside if inside and (not after_operator or random.random() < 0.5) else after_operator
        if not positions:
            return self.code + "\nval x = (\n"
        return self.code[:random.choice(positions)].rstrip(" ") + "\n"

    def err_double_operator(self) -> str:
        """Dva binarni operatory za sebou (a * / b); '+' a '-' jako druhe ne, ty jsou i unarni."""
        clean_code = self._strip_comments_and_strings(self.code)
        matches = list(self._BINARY_OPERATOR.finditer(clean_code))
        if not matches:
            return self.err_drop_keyword()
        match = random.choice(matches)
        end = match.end(1)
        return self.code[:end] + " " + random.choice(["*", "/", "==", "<=", ">", "!="]) + self.code[end:]

    KEYWORDS = ["while", "if", "then", "else", "def", "do", "return", "val", "var", "import"]

    def err_keyword_as_name(self) -> str:
        """Pouzije klicove slovo jako jmeno promenne, parametru nebo funkce."""
        clean_code = self._strip_comments_and_strings(self.code)
        matches = list(re.finditer(r"\b(?:val|var|def)\s+([A-Za-z_]\w*)|[(,]\s*([A-Za-z_]\w*)\s*:", clean_code))
        if not matches:
            return self.err_drop_keyword()
        match = random.choice(matches)
        group = 1 if match.group(1) else 2
        start, end = match.span(group)
        return self.code[:start] + random.choice(self.KEYWORDS) + self.code[end:]

    def err_typo_keyword(self) -> str:
        """Preklep v klicovem slove (val -> vall, then -> thne ...)."""
        typos = {"val": "vall", "var": "vra", "def": "deff", "while": "whlie", "then": "thne",
                 "do": "od", "else": "esle", "if": "iff", "import": "improt"}
        clean_code = self._strip_comments_and_strings(self.code)
        matches = list(re.finditer(r"\b(" + "|".join(typos) + r")\b", clean_code))
        if not matches:
            return self.err_drop_keyword()
        match = random.choice(matches)
        return self.code[:match.start()] + typos[match.group(1)] + self.code[match.end():]

    def err_separator(self) -> str:
        """Chybna carka v seznamu argumentu ci parametru: (a,, b), (, a), (a b)."""
        clean_code = self._strip_comments_and_strings(self.code)
        commas = [m.start() for m in re.finditer(r",", clean_code)]
        opens = [m.end() for m in re.finditer(r"\((?=\s*[^\s)])", clean_code)]
        choice = random.random()
        if commas and choice < 0.4:
            pos = random.choice(commas)
            return self.code[:pos] + ", ," + self.code[pos + 1:]
        if commas and choice < 0.7:
            pos = random.choice(commas)
            return self.code[:pos] + " " + self.code[pos + 1:]
        if opens:
            pos = random.choice(opens)
            return self.code[:pos] + ", " + self.code[pos:]
        return self.err_unbalanced_delimiters()

    def err_empty_block(self) -> str:
        """Smaze cele telo bloku (za then/do/else nebo '=' hlavicky funkce nic nezustane)."""
        statements = self._statement_lines()
        candidates = []
        for position, (index, indent) in enumerate(statements[:-1]):
            next_index, next_indent = statements[position + 1]
            if next_indent > indent:
                end = len(self.lines)
                for later_index, later_indent in statements[position + 1:]:
                    if later_indent <= indent:
                        end = later_index
                        break
                candidates.append((index + 1, end))
        if not candidates:
            return self.err_drop_keyword()
        start, end = random.choice(candidates)
        return "".join(self.lines[:start] + self.lines[end:])

    def err_unterminated(self) -> str:
        """Neukonceny retezec nebo blokovy komentar (lexikalni chyba)."""
        spans = [(start, end) for start, end in self._literal_spans(self.code)
                 if self.code.startswith(("/*", '"'), start) and not self.code.startswith('"""', start)]
        if not spans:
            return self.code + '\nval s = "\n'
        start, end = random.choice(spans)
        if self.code.startswith("/*", start):
            # Odstranime posledni '*/' komentare, vnoreny komentar pak zustane neuzavreny
            return self.code[:end - 2] + self.code[end:]
        return self.code[:end - 1] + self.code[end:]

    def err_bad_literal(self) -> str:
        """Chybny ciselny literal (lexikalni chyba)."""
        clean_code = self._strip_comments_and_strings(self.code)
        numbers = [m.span() for m in re.finditer(r"(?<![\w.])\d+(?![\w.])", clean_code)]
        bad = random.choice(["1_", "1e", "2e+", "3.4e-", "5__", "1_.5", "0x"])
        if not numbers:
            return self.code + f"\nval n = {bad}\n"
        start, end = random.choice(numbers)
        return self.code[:start] + bad + self.code[end:]

    def err_juxtaposition(self) -> str:
        """Dva prikazy na jednom radku bez stredniku (val a = 1 val b = 2)."""
        statements = self._statement_lines()
        candidates = []
        for (index, indent), (next_index, next_indent) in zip(statements, statements[1:]):
            next_text = self.lines[next_index].lstrip(" ")
            if indent == next_indent and next_index == index + 1 and \
                    re.match(r"(val|var|while|if|return)\b", next_text) and \
                    len(self.lines[index].rstrip()) == len(self._clean_line(index).rstrip()):
                candidates.append(index)
        if not candidates:
            return self.err_drop_keyword()
        index = random.choice(candidates)
        res = list(self.lines)
        res[index] = res[index].rstrip("\r\n") + " " + res[index + 1].lstrip(" ")
        res[index + 1] = ""
        return "".join(res)

    def err_assign_to_literal(self) -> str:
        """Prirazeni do literalu (1 = x); parser Scaly ho prijme, gramatika IFJ26 ne."""
        clean_code = self._strip_comments_and_strings(self.code)
        matches = [m for m in re.finditer(r"(?m)^( *)([A-Za-z_]\w*)( *=)(?![=>])", clean_code)
                   if m.group(2) not in self.RESERVED]
        if not matches:
            return self.err_drop_keyword()
        match = random.choice(matches)
        start, end = match.span(2)
        return self.code[:start] + random.choice(["1", '"s"', "2.5"]) + self.code[end:]

    def err_missing_colon(self) -> str:
        """Typ bez dvojtecky v parametru nebo za hlavickou funkce (def f() Int =)."""
        clean_code = self._strip_comments_and_strings(self.code)
        matches = [m.start(1) for m in re.finditer(r"[\w)] *(:) *[A-Z]", clean_code)]
        if not matches:
            return self.err_drop_keyword()
        pos = random.choice(matches)
        return self.code[:pos] + " " + self.code[pos + 1:]

    def _break_points(self, inside_parens: bool) -> list[tuple[int, int, int]]:
        """
        (line index, position, statement indent) where a statement line can be broken: after
        a binary operator outside parens, or after '(' / ',' / operator inside parens.
        """
        result = []
        continuation = self._continuation_lines()
        for index, indent, depth, code_end in self._code_lines():
            if indent is None or depth > 0 or index in continuation:
                continue
            clean = self._clean_line(index)[:code_end]
            if clean.lstrip().startswith(("import", "@")):
                continue
            candidates = [m.end() for m in self._BINARY_OPERATOR.finditer(clean)]
            if inside_parens:
                candidates += [m.end() for m in re.finditer(r"[(,] *", clean)]
            for pos in candidates:
                before = clean[:pos]
                opened = before.count("(") > before.count(")")
                if opened == inside_parens and clean[pos:].strip():
                    result.append((index, pos, indent))
        return result

    def err_trailing_operator(self) -> str:
        """Vyraz pokracuje za operatorem na dalsim radku mimo zavorky (zadani 1.2; Scala to povoli)."""
        points = self._break_points(inside_parens=False)
        if not points:
            return self.err_operator_line_start()
        index, pos, indent = random.choice(points)
        res = list(self.lines)
        line = res[index]
        res[index] = line[:pos].rstrip(" ") + "\n" + " " * (indent + random.randint(0, 6)) + line[pos:]
        return "".join(res)

    def err_paren_indent(self) -> str:
        """Radek uvnitr zavorek odsazeny nejvyse na zacatek logickeho radku (zadani 1.2; Scala to povoli)."""
        points = self._break_points(inside_parens=True)
        if not points:
            return self.err_trailing_operator()
        index, pos, indent = random.choice(points)
        res = list(self.lines)
        line = res[index]
        res[index] = line[:pos].rstrip(" ") + "\n" + " " * random.randint(0, indent) + line[pos:]
        return "".join(res)

    def err_tab_indent(self) -> str:
        """Tabulator na zacatku neprazdneho radku (lexikalni chyba 1; Scala ho povoli)."""
        statements = self._statement_lines()
        if not statements:
            return "\t" + self.code
        index, indent = random.choice(statements)
        res = list(self.lines)
        stripped = res[index].lstrip(" ")
        res[index] = random.choice(["\t", "\t" * max(indent // 2, 1), " " * indent + "\t"]) + stripped \
            if indent else "\t" + stripped
        return "".join(res)

    # Chyby, ktere IFJ26 odmita, ale parser Scaly je prijme, takze je oracle nepotvrdi:
    # Scala povoluje leading infix operatory, prirazeni do vyrazu, postfixove operatory
    # (a + * b je pro ni (a +) * b) a za then/do/else bere jako telo i neodsazeny dalsi radek
    STRICTER_THAN_SCALA = {"indent_err", "op_start_err", "assign_lit_err", "double_op_err", "empty_block_err",
                           "trailing_op_err", "paren_indent_err", "tab_indent_err"}

    def generate_error(self) -> tuple[str, str]:
        errors = [
            (self.err_indentation, "indent_err"),
            (self.err_unbalanced_delimiters, "delim_err"),
            (self.err_drop_keyword, "missing_kw"),
            (self.err_operator_line_start, "op_start_err"),
            (self.err_dedent_between, "dedent_err"),
            (self.err_truncate, "truncate_err"),
            (self.err_double_operator, "double_op_err"),
            (self.err_keyword_as_name, "kw_name_err"),
            (self.err_typo_keyword, "typo_kw_err"),
            (self.err_separator, "separator_err"),
            (self.err_empty_block, "empty_block_err"),
            (self.err_unterminated, "unterminated_err"),
            (self.err_bad_literal, "bad_literal_err"),
            (self.err_juxtaposition, "juxtapose_err"),
            (self.err_assign_to_literal, "assign_lit_err"),
            (self.err_missing_colon, "missing_colon_err"),
            (self.err_trailing_operator, "trailing_op_err"),
            (self.err_paren_indent, "paren_indent_err"),
            (self.err_tab_indent, "tab_indent_err"),
        ]
        fn, name = random.choice(errors)
        return fn(), name


ORACLE = Path(__file__).resolve().parent / "ScalaAstOracle.scala"


def run_oracle(files: list[Path], check_only: bool) -> dict[Path, str]:
    """
    Runs ScalaAstOracle once over all files (JVM startup is slow) and returns file -> status
    (OK, SYNTAX or CHYBA). Without check_only it also writes the .ast references.
    """
    if not files:
        return {}
    cmd = ["scala", "run", "--server=false", str(ORACLE), "--"] + (["--check"] if check_only else [])
    proc = subprocess.run(cmd + [str(f.resolve()) for f in files], capture_output=True, text=True)
    statuses = {}
    for line in proc.stdout.splitlines():
        match = re.match(r"(OK|SYNTAX|CHYBA)\s+(.+?)(?::\s|$)", line)
        if match:
            statuses[Path(match.group(2))] = match.group(1)
    if not statuses:
        raise RuntimeError(f"ScalaAstOracle nevratil zadny vysledek:\n{proc.stdout}\n{proc.stderr}")
    return statuses


def remove_test(scala_file: Path) -> None:
    for ext in (".scala", ".in", ".out", ".ast"):
        scala_file.with_suffix(ext).unlink(missing_ok=True)


def verify_with_oracle(valid: list[Path], errors: list[tuple[Path, str]]) -> None:
    """
    Validni testy: parser Scaly je musi prijmout (jinak mutace program rozbila a test se smaze);
    referencni .ast se vygeneruje znovu pro kazdy podporovany test, i pro prejmenovane identifikatory.
    Chybove testy: parser Scaly je musi odmitnout, jinak neni jiste, ze jde o chybu, a test se smaze.
    Vyjimkou jsou kategorie, ve kterych je IFJ26 prisnejsi nez Scala.
    """
    for test in valid:
        test.with_suffix(".ast").unlink(missing_ok=True)
    print(f"Oracle: overuji {len(valid)} validnich testu...")
    statuses = run_oracle(valid, check_only=False)
    broken = [t for t in valid if statuses.get(t.resolve()) == "SYNTAX"]
    unsupported = [t for t in valid if statuses.get(t.resolve()) == "CHYBA"]
    for test in broken:
        remove_test(test)
    print(f"  {len(valid) - len(broken) - len(unsupported)} s referencnim AST, {len(unsupported)} bez AST "
          f"(konstrukce mimo zaklad), {len(broken)} smazano (mutace je rozbila)")

    checkable = [(t, tag) for t, tag in errors if tag not in IFJCodeMutator.STRICTER_THAN_SCALA]
    print(f"Oracle: overuji {len(checkable)} chybovych testu ({len(errors) - len(checkable)} kategorii "
          f"prisnejsich nez Scala se neoveruje)...")
    statuses = run_oracle([t for t, _ in checkable], check_only=True)
    dropped: dict[str, int] = {}
    for test, tag in checkable:
        if statuses.get(test.resolve()) != "SYNTAX":
            remove_test(test)
            dropped[tag] = dropped.get(tag, 0) + 1
    summary = ", ".join(f"{tag} {count}x" for tag, count in sorted(dropped.items())) or "zadny"
    print(f"  smazano {sum(dropped.values())} nepotvrzenych chyb (Scala je prijala): {summary}")


def main():
    parser = argparse.ArgumentParser(description="IFJ26 Seed-based Fuzzer")
    parser.add_argument("--seeds-dir", type=str, nargs="+", default=["."], help="Adresare se soubory .scala, .in, .out")
    parser.add_argument("--out-dir", type=str, default="generated_tests", help="Vystupni adresar")
    parser.add_argument("--valid-count", type=int, default=15, help="Pocet validnich testu")
    parser.add_argument("--invalid-count", type=int, default=30, help="Pocet syntaktickych chyb")
    parser.add_argument("--seed", type=int, default=None, help="Random generator seed for a reproducible run")
    parser.add_argument("--clean", action="store_true", help="Delete previously generated test_valid_* and test_error_* in the output directory first")
    parser.add_argument("--tag", type=str, default="",
                        help="Stitek do nazvu testu (test_valid_<tag>_001_...), aby se nove testy neprepsaly se starymi")
    parser.add_argument("--copy-seeds", action="store_true",
                        help="Zkopirovat i samotne seedy jako validni testy (beze zmeny)")
    parser.add_argument("--oracle", action="store_true",
                        help="Overit testy parserem Scaly (ScalaAstOracle): smazat rozbite validni a nepotvrzene "
                             "chybove testy a vygenerovat referencni .ast")
    args = parser.parse_args()

    if args.seed is not None:
        random.seed(args.seed)

    # Vyfiltrujeme pripadny knihovni soubor ifj26.scala, aby nebyl mutovan jako test
    scala_files = sorted(f for d in args.seeds_dir for f in Path(d).glob("*.scala") if f.name != "ifj26.scala")
    if not scala_files:
        print(f"Chyba: Zadne .scala soubory v {args.seeds_dir}.")
        return

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    if args.clean:
        for pattern in ("test_valid_*", "test_error_*"):
            for old_file in out_dir.glob(pattern):
                old_file.unlink()

    print(f"Nalezeno {len(scala_files)} seedu.")
    tag = f"{args.tag}_" if args.tag else ""
    valid_tests: list[Path] = []
    error_tests: list[tuple[Path, str]] = []

    def write_valid(base_name: str, src: Path, code: str, same_ast: bool) -> None:
        (out_dir / f"{base_name}.scala").write_text(code, encoding="utf-8")
        valid_tests.append(out_dir / f"{base_name}.scala")

        # Mutations that preserve the AST can be compared with the seed's reference tree
        ast_file = src.with_suffix(".ast")
        ast_target = out_dir / f"{base_name}.ast"
        if same_ast and ast_file.exists():
            shutil.copy(ast_file, ast_target)
        else:
            # A reference from a previous run would not match the new code
            ast_target.unlink(missing_ok=True)

        # Zkopirovani prislusneho .in a .out souboru, pokud existuji
        for ext in [".in", ".out"]:
            ref_file = src.with_suffix(ext)
            target_file = out_dir / f"{base_name}{ext}"
            if ref_file.exists():
                shutil.copy(ref_file, target_file)
            else:
                target_file.touch()

    if args.copy_seeds:
        for src in scala_files:
            write_valid(f"test_valid_{tag}seed_{src.stem}", src, src.read_text(encoding="utf-8"), True)

    # 1. Generovani validnich vzorku
    for i in range(1, args.valid_count + 1):
        src = random.choice(scala_files)
        mutator = IFJCodeMutator(src.read_text(encoding="utf-8"))
        code, same_ast = mutator.generate_valid()
        write_valid(f"test_valid_{tag}{i:03d}_{src.stem}", src, code, same_ast)

    # 2. Generovani syntakticky chybnych vzorku
    for i in range(1, args.invalid_count + 1):
        src = random.choice(scala_files)
        mutator = IFJCodeMutator(src.read_text(encoding="utf-8"))
        code, err_tag = mutator.generate_error()

        base_name = f"test_error_{tag}{i:03d}_{err_tag}_{src.stem}"
        (out_dir / f"{base_name}.scala").write_text(code, encoding="utf-8")
        (out_dir / f"{base_name}.in").touch()
        error_tests.append((out_dir / f"{base_name}.scala", err_tag))

    if args.oracle:
        verify_with_oracle(valid_tests, error_tests)

    remaining = sum(t.exists() for t in valid_tests) + sum(t.exists() for t, _ in error_tests)
    print(f"Hotovo. V '{out_dir}/' je {remaining} novych testu.")


if __name__ == "__main__":
    main()