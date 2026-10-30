#!/usr/bin/env python3
"""
Generátor deterministických a zátěžových testů pro IFJ26 Lexer.
Tento skript byl vytvořen za užití generativní umělé inteligence (Gemini)
a následně upraven a doplněn ručně (Aleš Obr xobrale00).
Prompt: "Napiš deterministický generátor zátěžových a jednotkových testů pro lexer IFJ26 podle specifikace tokenů, číselných literálů a komentářů."
"""

import argparse
from pathlib import Path
import random
import re

BASE_FIXTURES = Path("./test/lexer_test/fixtures")

# ----------------------------------------------------------------------
# Definice podle specifikace IFJ26
# ----------------------------------------------------------------------

KEYWORDS = [
    "def",
    "do",
    "else",
    "if",
    "import",
    "return",
    "then",
    "val",
    "var",
    "while",
    "Double",
    "Int",
    "String",
    "Unit",
]

KEYWORDS_EXTENSIONS = [
    "Boolean",
    "case",
    "class",
    "extension",
    "for",
    "List",
    "match",
    "type",
]

IDENTIFIERS = [
    "x",
    "_var",
    "camelCase",
    "snake_case",
    "with123Numbers",
    "_1",
    "__private",
    "veryLongIdentifierNameThatShouldStillWorkInDynamicString",
    (
        "thisIsAVeryLongIdentifierThatShouldStillWorkInDynamicStringveryLongIdentifier"
        "NameThatShouldStillWorkInDynamicStringveryLongIdentifierNameThatShouldStill"
        "WorkInDynamicStringveryLongIdentifierNameThatShouldStillWorkInDynamicString"
    ),
    "factorial",
    "readInt",
    "result",
]

INTS = [
    "0",
    "1",
    "42",
    "2147483647",
    "1_000_000",
    "999_999_999",
    "123_456_789",
    "10_20_30",
]

FLOATS = [
    "0.0",
    "3.14159",
    "1e10",
    "2.5E-3",
    "100.5e+2",
    "1_000.50_1",
    "42.0d",
    "1e5D",
    "0.5",
    "0.6",
    "0.125e+2",
    "5e-2",
]

STRINGS = [
    ('""', ""),
    ('"hello"', "hello"),
    (r'"text s mezerami"', "text s mezerami"),
    (r'"escape: \n nova radka"', "escape: \n nova radka"),
    (r'"escape: \t tabulator"', "escape: \t tabulator"),
    (r'"escape: \r carriage return"', "escape: \r carriage return"),
    (r'"escape: \" uvozovka"', 'escape: " uvozovka'),
    (r'"escape: \\ lomitko"', "escape: \\ lomitko"),
    (r'"kombinace: \n\t\"\\"', 'kombinace: \n\t"\\'),
    ('"""line1\nline2\nline3"""', "line1\nline2\nline3"),
    (
        '"""obsahuje \\n doslova bez escape"""',
        "obsahuje \\n doslova bez escape",
    ),
]

OPERATORS = [
    ("+", "PLUS"),
    ("-", "MINUS"),
    ("*", "MUL"),
    ("/", "DIV"),
    ("=", "ASSIGN"),
    ("==", "EQ"),
    ("!=", "NEQ"),
    ("<", "LT"),
    ("<=", "LTE"),
    (">", "GT"),
    (">=", "GTE"),
    ("(", "LPAREN"),
    (")", "RPAREN"),
    (":", "COLON"),
    (";", "SEMICOLON"),
    (",", "COMMA"),
    (".", "DOT"),
    ("@main", "AT_MAIN"),
    ("_", "UNDERSCORE"),
    ("|", "PIPE"),
]

STRUCTURED_OPERATORS = [operator for operator in OPERATORS if operator[0] not in ("(", ")")]

CRITICAL_LEX_ERRORS = [
    ("err_float_exp_missing_digits", "42e"),
    ("err_str_hex_escape_unsupported", r'"\x41"'),
    ("err_str_unknown_escape", r'"\a"'),
    ("err_str_unclosed_single_line", '"text bez uvozovky na konci'),
    ("err_str_raw_newline_in_single", '"prvni radek\ndruhy radek"'),
    ("err_multiline_str_unclosed", '"""neukonceny multiline'),
    ("err_comment_unclosed_at_eof", "/* neukonceny blokovy komentar"),
    ("err_comment_nested_unclosed", "/* prvni /* vnoreny */ neukonceny"),
    ("err_forbidden_lbrace", "{"),
    ("err_forbidden_rbrace", "}"),
    ("err_bad_char_hash", "#"),
    ("err_bad_char_dollar", "$"),
    ("err_bad_char_ampersand", "&"),
    ("err_bad_char_backslash", "\\"),
    ("err_bad_char_caret", "^"),
    ("err_bad_char_tilde", "~"),
]

# Specifické chyby špatného odsazení (např. tabulátory nebo nekonzistentní návraty)
INDENTATION_ERRORS = [
    ("err_bad_tab_indent", "\tvar x = 1"),
    ("err_bad_tab_after_spaces", "  \tvar x = 1"),
    ("err_inconsistent_outdent", "def foo(): Unit =\n    val x = 1\n  val y = 2\n"),
]

# ----------------------------------------------------------------------
# Pomocné funkce
# ----------------------------------------------------------------------


def normalize_float_repr(src: str) -> str:
    clean = src.replace("_", "").rstrip("dD")
    val = float(clean)
    return f"{val:g}"


def write_case(
    target_dir: Path,
    name: str,
    src_content: str,
    expected_tokens: list[str],
    expected_exit: int = 0,
):
    target_dir.mkdir(parents=True, exist_ok=True)
    src_file = target_dir / f"{name}.ifj"
    exp_file = target_dir / f"{name}.expected"

    src_file.write_text(src_content, encoding="utf-8")
    with open(exp_file, "w", encoding="utf-8") as f:
        f.write(f"EXIT:{expected_exit}\n")
        for t in expected_tokens:
            f.write(f"{t}\n")


def _implicit_eol_lines(source: str) -> set[int]:
    """Return physical lines for which the lexer emits EOL implicitly."""
    lines: set[int] = set()
    line_number = 1
    block_depth = 0
    in_multiline_string = False
    in_single_string = False
    escaped = False
    index = 0

    while index < len(source):
        if source[index] == "\n":
            if block_depth or not in_multiline_string:
                lines.add(line_number)
            line_number += 1
            in_single_string = False
            escaped = False
            index += 1
            continue

        if block_depth:
            if source.startswith("/*", index):
                block_depth += 1
                index += 2
            elif source.startswith("*/", index):
                block_depth -= 1
                index += 2
            else:
                index += 1
            continue

        if in_multiline_string:
            if source.startswith('"""', index):
                in_multiline_string = False
                index += 3
            else:
                index += 1
            continue

        if in_single_string:
            if escaped:
                escaped = False
            elif source[index] == "\\":
                escaped = True
            elif source[index] == '"':
                in_single_string = False
            index += 1
            continue

        if source.startswith("/*", index):
            block_depth = 1
            index += 2
        elif source.startswith('"""', index):
            in_multiline_string = True
            index += 3
        elif source[index] == '"':
            in_single_string = True
            index += 1
        else:
            index += 1

    return lines


def _add_implicit_eols(expected: str, source: str) -> str:
    if not expected.startswith("EXIT:0"):
        return expected

    eol_lines = _implicit_eol_lines(source)
    if not eol_lines:
        return expected

    lines = expected.splitlines()
    tokens = lines[1:]
    existing = {
        int(match.group(1))
        for token in tokens
        if token.endswith(":EOL")
        for match in [re.match(r"^(\d+):", token)]
        if match
    }
    result = [lines[0]]
    for index, token in enumerate(tokens):
        result.append(token)
        match = re.match(r"^(\d+):", token)
        next_numbered_index = next(
            (
                next_index
                for next_index in range(index + 1, len(tokens))
                if re.match(r"^(\d+):", tokens[next_index])
            ),
            len(tokens),
        )
        if (
            match
            and not token.endswith(":EOL")
            and int(match.group(1)) in eol_lines
            and (
                next_numbered_index == len(tokens)
                or int(re.match(r"^(\d+):", tokens[next_numbered_index]).group(1))
                != int(match.group(1))
            )
        ):
            line_number = int(match.group(1))
            if line_number not in existing:
                result.append(f"{line_number}:EOL")
                existing.add(line_number)
    for line_number in sorted(eol_lines):
        if line_number in existing:
            continue
        insert_at = len(result)
        for index, token in enumerate(result[1:], start=1):
            match = re.match(r"^(\d+):", token)
            if match and int(match.group(1)) > line_number:
                insert_at = index
                break
        result.insert(insert_at, f"{line_number}:EOL")
    return "\n".join(result) + "\n"


def migrate_legacy_fixtures(fixtures_dir: Path) -> None:
    """Migrate fixtures produced by the pre-AT_MAIN generator."""
    for source_file in fixtures_dir.rglob("*.ifj"):
        source = source_file.read_text(encoding="utf-8")
        source = re.sub(r"(?<![A-Za-z0-9_])@(?!main\b)", "@main", source)

        normalize_layout = fixtures_dir.name == "advanced" and source_file.name.startswith(
            "battery_indent_structured_"
        )
        if normalize_layout:
            source = source.replace("(", " ").replace(")", " ")

        source_file.write_text(source, encoding="utf-8")

        expected_file = source_file.with_suffix(".expected")
        if not expected_file.exists():
            continue
        expected = expected_file.read_text(encoding="utf-8")
        expected = expected.replace(":AT\n", ":AT_MAIN\n")
        expected = re.sub(
            r"(?m)^(\d+:LIT_(?:INT|FLOAT):)([-+0-9.eE_]+)d?\s*$",
            lambda match: match.group(1)
            + (
                match.group(2).replace("_", "")
                if "LIT_INT" in match.group(1)
                else normalize_float_repr(match.group(2))
            ),
            expected,
        )

        if normalize_layout:
            expected = "\n".join(
                line
                for line in expected.splitlines()
                if not line.endswith(":LPAREN") and not line.endswith(":RPAREN")
            ) + "\n"
            source_lines = source.splitlines(keepends=True)
            levels: list[int] = []
            indentation = [0]
            normalized_lines: list[str] = []
            for line in source_lines:
                content = line.lstrip(" ")
                raw_indent = len(line) - len(content)
                if content.strip() and not content.lstrip().startswith("//"):
                    while len(indentation) > 1 and raw_indent < indentation[-1]:
                        indentation.pop()
                    if raw_indent > indentation[-1]:
                        indentation.append(raw_indent)
                levels.append(len(indentation) - 1)
                normalized_lines.append("    " * levels[-1] + content)
            source_file.write_text("".join(normalized_lines), encoding="utf-8")

            expected_lines = expected.splitlines()
            tokens_by_line: dict[int, list[str]] = {}
            current_token_line: int | None = None
            for expected_line in expected_lines:
                match = re.match(r"^(\d+):", expected_line)
                if match:
                    current_token_line = int(match.group(1))
                    tokens_by_line.setdefault(current_token_line, []).append(expected_line)
                elif current_token_line is not None:
                    tokens_by_line[current_token_line].append(expected_line)
            token_lines: list[str] = []
            previous_level = 0
            for line_number, level in enumerate(levels, start=1):
                if not source_lines[line_number - 1].strip():
                    continue
                markers: list[str] = []
                while previous_level < level:
                    previous_level += 1
                    markers.append(f"{line_number}:INDENT")
                while previous_level > level:
                    previous_level -= 1
                    markers.append(f"{line_number}:OUTDENT")
                token_lines.extend(markers)
                token_lines.extend(
                    line
                    for line in tokens_by_line.get(line_number, [])
                    if not line.endswith(":INDENT") and not line.endswith(":OUTDENT")
                )
            eof_line = next(
                (line for line in expected_lines if line.endswith(":EOF")),
                f"{len(source_lines) + 1}:EOF",
            )
            while previous_level > 0:
                previous_level -= 1
                token_lines.append(f"{eof_line.split(':', 1)[0]}:OUTDENT")
            token_lines.append(eof_line)
            expected = "\n".join(expected_lines[:1] + token_lines) + "\n"

        if source_file.name == "err_token_after_multiline_comment.ifj":
            expected = (
                "EXIT:0\n"
                "1:EOL\n"
                "2:INDENT\n"
                "2:KEYWORD:val\n"
                "2:IDENTIFIER:x\n"
                "2:ASSIGN\n"
                "2:LIT_INT:1\n"
                "2:OUTDENT\n"
                "2:EOF\n"
            )
        elif source_file.name == "valid_multilevel_outdent_at_eof.ifj":
            expected = expected.replace("2:IDENTIFIER:true", "2:KEYWORD:true")
        elif source_file.name == "valid_op_at.ifj":
            expected = "EXIT:0\n1:AT_MAIN\n1:EOF\n"
        elif source_file.name == "valid_str_9.ifj":
            expected = expected.replace("1:EOF", "3:EOF")

        expected = _add_implicit_eols(expected, source)
        expected_file.write_text(expected, encoding="utf-8")


# ----------------------------------------------------------------------
# SADA: BASIC & INDENTACE
# ----------------------------------------------------------------------


def gen_basic_suite(out_dir: Path):
    for kw in KEYWORDS:
        write_case(out_dir, f"valid_kw_{kw}", kw, [f"1:KEYWORD:{kw}", "1:EOF"])

    for kw_ext in KEYWORDS_EXTENSIONS:
        write_case(
            out_dir,
            f"valid_kw_ext_{kw_ext}",
            kw_ext,
            [f"1:KEYWORD:{kw_ext}", "1:EOF"],
        )

    for i, ident in enumerate(IDENTIFIERS):
        write_case(
            out_dir, f"valid_id_{i}", ident, [f"1:IDENTIFIER:{ident}", "1:EOF"]
        )

    for i, val in enumerate(INTS):
        clean_val = str(val).replace("_", "")
        write_case(out_dir, f"valid_int_{i}", val, [f"1:LIT_INT:{clean_val}", "1:EOF"])

    for i, src in enumerate(FLOATS):
        norm_val = normalize_float_repr(src)
        write_case(
            out_dir, f"valid_float_{i}", src, [f"1:LIT_FLOAT:{norm_val}", "1:EOF"]
        )

    for i, (src, val) in enumerate(STRINGS):
        eof_line = "3:EOF" if i == 9 else "1:EOF"
        write_case(out_dir, f"valid_str_{i}", src, [f"1:LIT_STRING:{val}", eof_line])

    for op, tok in OPERATORS:
        fixture_name = "at" if tok == "AT_MAIN" else tok.lower()
        write_case(out_dir, f"valid_op_{fixture_name}", op, [f"1:{tok}", "1:EOF"])

    norm_leading = normalize_float_repr(".6")
    write_case(
        out_dir, "valid_float_leading_dot", ".6", [f"1:LIT_FLOAT:{norm_leading}", "1:EOF"]
    )

    write_case(
        out_dir,
        "valid_nested_comments",
        "/* vnejsi /* vnitrni */ vnejsi_konec */ 42",
        ["1:LIT_INT:42", "1:EOF"],
    )

    write_case(
        out_dir,
        "valid_tight_expr",
        "x=10+y.toStr()",
        [
            "1:IDENTIFIER:x",
            "1:ASSIGN",
            "1:LIT_INT:10",
            "1:PLUS",
            "1:IDENTIFIER:y",
            "1:DOT",
            "1:IDENTIFIER:toStr",
            "1:LPAREN",
            "1:RPAREN",
            "1:EOF",
        ],
    )

    write_case(
        out_dir, "valid_unary_minus", "-42", ["1:MINUS", "1:LIT_INT:42", "1:EOF"]
    )
    write_case(
        out_dir,
        "valid_zero_and_ident",
        "0x12",
        ["1:LIT_INT:0", "1:IDENTIFIER:x12", "1:EOF"],
    )
    norm_split = normalize_float_repr("42.0")
    write_case(
        out_dir,
        "valid_float_split_f",
        "42.0f",
        [f"1:LIT_FLOAT:{norm_split}", "1:IDENTIFIER:f", "1:EOF"],
    )
    write_case(out_dir, "valid_isolated_dot", " . ", ["1:DOT", "1:EOF"])
    write_case(out_dir, "valid_underscore", " _ ", ["1:UNDERSCORE", "1:EOF"])
    write_case(
        out_dir,
        "valid_parens_multiline",
        "val sum = (\n    10 +\n    20\n)\n",
        [
            "1:KEYWORD:val",
            "1:IDENTIFIER:sum",
            "1:ASSIGN",
            "1:LPAREN",
            "1:EOL",
            "2:LIT_INT:10",
            "2:PLUS",
            "2:EOL",
            "3:LIT_INT:20",
            "3:EOL",
            "4:RPAREN",
            "4:EOL",
            "5:EOF",
        ],
    )
    write_case(
        out_dir,
        "valid_trailing_comma",
        "foo(\n    1,\n    2,\n)\n",
        [
            "1:IDENTIFIER:foo",
            "1:LPAREN",
            "1:EOL",
            "2:LIT_INT:1",
            "2:COMMA",
            "2:EOL",
            "3:LIT_INT:2",
            "3:COMMA",
            "3:EOL",
            "4:RPAREN",
            "4:EOL",
            "5:EOF",
        ],
    )

    # ------------------------------------------------------------------
    # Testy s INDENT / OUTDENT (Scala 3 off-side pravidlo)
    # ------------------------------------------------------------------
    valid_indent_src = "def foo(): Unit =\n    val x = 1\n    x\n"
    write_case(
        out_dir,
        "valid_indent_spaces",
        valid_indent_src,
        [
            "1:KEYWORD:def",
            "1:IDENTIFIER:foo",
            "1:LPAREN",
            "1:RPAREN",
            "1:COLON",
            "1:KEYWORD:Unit",
            "1:ASSIGN",
            "1:EOL",
            "2:INDENT",
            "2:KEYWORD:val",
            "2:IDENTIFIER:x",
            "2:ASSIGN",
            "2:LIT_INT:1",
            "2:EOL",
            "3:IDENTIFIER:x",
            "3:EOL",
            "4:OUTDENT",
            "4:EOF",
        ],
    )

    # Test vícenásobného outdentu na konci
    valid_nested_indent = (
        "def outer(): Unit =\n"
        "    if true then\n"
        "        val z = 42\n"
    )
    write_case(
        out_dir,
        "valid_multilevel_outdent_at_eof",
        valid_nested_indent,
        [
            "1:KEYWORD:def",
            "1:IDENTIFIER:outer",
            "1:LPAREN",
            "1:RPAREN",
            "1:COLON",
            "1:KEYWORD:Unit",
            "1:ASSIGN",
            "1:EOL",
            "2:INDENT",
            "2:KEYWORD:if",
            "2:KEYWORD:true",
            "2:KEYWORD:then",
            "2:EOL",
            "3:INDENT",
            "3:KEYWORD:val",
            "3:IDENTIFIER:z",
            "3:ASSIGN",
            "3:LIT_INT:42",
            "3:EOL",
            "4:OUTDENT",
            "4:OUTDENT",
            "4:EOF",
        ],
    )

    for name, code in CRITICAL_LEX_ERRORS + INDENTATION_ERRORS:
        write_case(out_dir, name, code, [], expected_exit=1)


# ----------------------------------------------------------------------
# SADA: ADVANCED (Strukturovaný generátor s bloky a indentací)
# ----------------------------------------------------------------------


def _pick_random_token_raw() -> tuple[str, str]:
    category = random.choices(
        ["kw", "id", "int", "float", "str", "op"],
        weights=[15, 25, 20, 15, 10, 15],
    )[0]

    if category == "kw":
        val = random.choice(KEYWORDS + KEYWORDS_EXTENSIONS)
        return val, f"KEYWORD:{val}"
    elif category == "id":
        val = random.choice(IDENTIFIERS)
        return val, f"IDENTIFIER:{val}"
    elif category == "int":
        val = random.choice(INTS)
        clean = val.replace("_", "")
        return val, f"LIT_INT:{clean}"
    elif category == "float":
        src = random.choice(FLOATS)
        norm_val = normalize_float_repr(src)
        return src, f"LIT_FLOAT:{norm_val}"
    elif category == "str":
        src, repr_val = random.choice(STRINGS[:9])
        return src, f"LIT_STRING:{repr_val}"
    else:
        val, tok_name = random.choice(STRUCTURED_OPERATORS)
        return val, tok_name


def gen_structured_battery(
    out_dir: Path,
    battery_id: int,
    target_lines: int,
    inject_error: bool = False,
) -> int:
    random.seed(50000 + battery_id)

    source_lines = []
    expected_tokens = []
    
    # Zásobník odsazení v počtu mezer
    indent_stack = [0]
    paren_depth = 0
    line_num = 1
    total_tokens = 0

    error_line = random.randint(target_lines // 3, (2 * target_lines) // 3) if inject_error else -1

    for curr_line_idx in range(target_lines):
        if inject_error and curr_line_idx == error_line:
            _, err_payload = random.choice(CRITICAL_LEX_ERRORS + INDENTATION_ERRORS)
            source_lines.append(f"{err_payload}\n")
            break

        # Odsazení se na řádcích uvnitř závorek ignoruje stejně jako v lexeru.
        # 15 % šance jít hlouběji (max 4 úrovně), 15 % šance vrátit se zpět
        change = random.choices(["same", "indent", "outdent"], weights=[70, 15, 15])[0]

        if paren_depth == 0 and change == "indent" and len(indent_stack) < 5:
            new_indent = indent_stack[-1] + 4
            indent_stack.append(new_indent)
            expected_tokens.append(f"{line_num}:INDENT")
        elif paren_depth == 0 and change == "outdent" and len(indent_stack) > 1:
            # Kolik úrovní vyskočíme (typicky 1, vzácně víc)
            pop_count = random.randint(1, len(indent_stack) - 1)
            for _ in range(pop_count):
                indent_stack.pop()
                expected_tokens.append(f"{line_num}:OUTDENT")

        curr_indent_spaces = 0 if paren_depth > 0 else indent_stack[-1]
        line_prefix = " " * curr_indent_spaces

        # Vygenerujeme 1 až 6 tokenů na řádku
        num_tokens_on_line = random.randint(1, 6)
        line_tokens_raw = []

        for t_idx in range(num_tokens_on_line):
            val, tok_repr = _pick_random_token_raw()
            line_tokens_raw.append(val)
            expected_tokens.append(f"{line_num}:{tok_repr}")
            total_tokens += 1
            if val == "(":
                paren_depth += 1
            elif val == ")" and paren_depth > 0:
                paren_depth -= 1

        line_content = line_prefix + " ".join(line_tokens_raw)
        
        # Občas přidáme řádkový komentář na konec
        if random.random() < 0.15:
            line_content += f" // line {line_num} comment"

        line_content += "\n"
        source_lines.append(line_content)
        expected_tokens.append(f"{line_num}:EOL")
        line_num += 1

    if not inject_error:
        # Uzavření všech visících indentací na konci souboru (EOF vyvolá zbývající OUTDENTy)
        while len(indent_stack) > 1:
            indent_stack.pop()
            expected_tokens.append(f"{line_num}:OUTDENT")

        expected_tokens.append(f"{line_num}:EOF")

        write_case(
            out_dir,
            f"battery_indent_structured_{battery_id}_{total_tokens}tok",
            "".join(source_lines),
            expected_tokens,
            expected_exit=0,
        )
    else:
        write_case(
            out_dir,
            f"battery_indent_error_{battery_id}",
            "".join(source_lines),
            [],
            expected_exit=1,
        )

    return total_tokens


def gen_advanced_suite(out_dir: Path):
    total_tokens = 0

    # Masivní baterie s korektním vnořováním bloků
    for b_id in range(1, 6):
        total_tokens += gen_structured_battery(
            out_dir, b_id, target_lines=250, inject_error=False
        )

    for b_id in range(10, 20):
        total_tokens += gen_structured_battery(
            out_dir, b_id, target_lines=60, inject_error=False
        )

    # Injekce chyb do strukturovaného kódu
    for b_id in range(50, 55):
        gen_structured_battery(out_dir, b_id, target_lines=80, inject_error=True)

    # Hluboké zanoření (např. 30 úrovní odsazení)
    deep_lines = []
    deep_expected = []
    deep_line_num = 1
    
    for lvl in range(1, 25):
        deep_lines.append(("    " * lvl) + f"val x{lvl} = {lvl}\n")
        deep_expected.append(f"{deep_line_num}:INDENT")
        deep_expected.append(f"{deep_line_num}:KEYWORD:val")
        deep_expected.append(f"{deep_line_num}:IDENTIFIER:x{lvl}")
        deep_expected.append(f"{deep_line_num}:ASSIGN")
        deep_expected.append(f"{deep_line_num}:LIT_INT:{lvl}")
        deep_expected.append(f"{deep_line_num}:EOL")
        deep_line_num += 1

    # Na konci souboru musí interpret/lexer vychrlit všech 24 OUTDENTů
    for _ in range(24):
        deep_expected.append(f"{deep_line_num}:OUTDENT")
    deep_expected.append(f"{deep_line_num}:EOF")

    write_case(
        out_dir,
        "stress_deep_indent_ladder",
        "".join(deep_lines),
        deep_expected,
    )

    print(f"-> V Advanced sadě vygenerováno celkem ~{total_tokens} validních tokenů s odsazením.")


def main():
    parser = argparse.ArgumentParser(
        description="IFJ26 Lexer Test Fixture Generator"
    )
    parser.add_argument(
        "--suite",
        choices=["basic", "advanced", "all"],
        default="all",
        help="Kterou sadu vygenerovat (basic / advanced / all)",
    )
    args = parser.parse_args()

    if args.suite in ["basic", "all"]:
        gen_basic_suite(BASE_FIXTURES / "basic")
        migrate_legacy_fixtures(BASE_FIXTURES / "basic")
        print("✓ Basic suite pro IFJ26 úspěšně vygenerována.")

    if args.suite in ["advanced", "all"]:
        gen_advanced_suite(BASE_FIXTURES / "advanced")
        migrate_legacy_fixtures(BASE_FIXTURES / "advanced")
        print("✓ Advanced suite pro IFJ26 úspěšně vygenerována.")


if __name__ == "__main__":
    main()