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
    ("@", "AT"),
    ("_", "UNDERSCORE"),
    ("|", "PIPE"),
]

CRITICAL_LEX_ERRORS = [
    ("err_float_exp_missing_digits", "42e"),
    ("err_str_hex_escape_unsupported", r'"\x41"'),
    ("err_str_unknown_escape", r'"\a"'),
    ("err_str_unclosed_single_line", '"text bez uvozovky na konci'),
    ("err_str_raw_newline_in_single", '"prvni radek\ndruhy radek"'),
    ("err_multiline_str_unclosed", '"""neukonceny multiline'),
    ("err_comment_unclosed_at_eof", "/* neukonceny blokovy komentar"),
    ("err_comment_nested_unclosed", "/* prvni /* vnoreny */ neukonceny"),
    ("err_token_after_multiline_comment", "/* \n */ val x = 1"),
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
        write_case(out_dir, f"valid_str_{i}", src, [f"1:LIT_STRING:{val}", "1:EOF"])

    for op, tok in OPERATORS:
        write_case(out_dir, f"valid_op_{tok.lower()}", op, [f"1:{tok}", "1:EOF"])

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
            "2:IDENTIFIER:true",
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
        val, tok_name = random.choice(OPERATORS)
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
    line_num = 1
    total_tokens = 0

    error_line = random.randint(target_lines // 3, (2 * target_lines) // 3) if inject_error else -1

    for curr_line_idx in range(target_lines):
        if inject_error and curr_line_idx == error_line:
            _, err_payload = random.choice(CRITICAL_LEX_ERRORS + INDENTATION_ERRORS)
            source_lines.append(f"{err_payload}\n")
            break

        # Rozhodnutí o změně úrovně odsazení
        # 15 % šance jít hlouběji (max 4 úrovně), 15 % šance vrátit se zpět
        change = random.choices(["same", "indent", "outdent"], weights=[70, 15, 15])[0]

        if change == "indent" and len(indent_stack) < 5:
            new_indent = indent_stack[-1] + 4
            indent_stack.append(new_indent)
            expected_tokens.append(f"{line_num}:INDENT")
        elif change == "outdent" and len(indent_stack) > 1:
            # Kolik úrovní vyskočíme (typicky 1, vzácně víc)
            pop_count = random.randint(1, len(indent_stack) - 1)
            for _ in range(pop_count):
                indent_stack.pop()
                expected_tokens.append(f"{line_num}:OUTDENT")

        curr_indent_spaces = indent_stack[-1]
        line_prefix = " " * curr_indent_spaces

        # Vygenerujeme 1 až 6 tokenů na řádku
        num_tokens_on_line = random.randint(1, 6)
        line_tokens_raw = []

        for t_idx in range(num_tokens_on_line):
            val, tok_repr = _pick_random_token_raw()
            line_tokens_raw.append(val)
            expected_tokens.append(f"{line_num}:{tok_repr}")
            total_tokens += 1

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
        print("✓ Basic suite pro IFJ26 úspěšně vygenerována.")

    if args.suite in ["advanced", "all"]:
        gen_advanced_suite(BASE_FIXTURES / "advanced")
        print("✓ Advanced suite pro IFJ26 úspěšně vygenerována.")


if __name__ == "__main__":
    main()