#!/usr/bin/env python3
"""
Generátor deterministických a zátěžových testů pro IFJ26 Lexer.
Tento skript byl vytvořen za užití generativní umělé inteligence (Gemini 3.8 flash) a následně upraven a doplněn ručně (Aleš Obr xobrale00)
"""

import argparse
import random
from pathlib import Path

BASE_FIXTURES = Path("./test/lexer_test/fixtures")

# ----------------------------------------------------------------------
# Definice podle specifikace IFJ26
# ----------------------------------------------------------------------

# Základní klíčová slova IFJ26
KEYWORDS = [
    "def", "do", "else", "if", "import", "return", 
    "then", "val", "var", "while",
    # Vestavěné typy vystupující jako klíčová slova
    "Double", "Int", "String", "Unit"
]

# Klíčová slova pro rozšíření (překladač je nesmí brát jako běžné identifikátory)
KEYWORDS_EXTENSIONS = [
    "Boolean", "case", "class", "extension", "for", "List", "match", "type"
]

IDENTIFIERS = [
    "x", "_var", "camelCase", "snake_case", "with123Numbers",
    "_1", "__private", "veryLongIdentifierNameThatShouldStillWorkInDynamicString",
    "factorial", "readInt", "result"
]

# Celá čísla (Int) – podpora oddělovačů _, zákaz úvodních nul (kromě samostatné 0)
INTS = [
    "0", "1", "42", "1_000_000", "999_999_999", 
    "123_456_789_012_345", "10_20_30"
]

# Desetinná čísla (Double) – exponenty, volitelné d/D, podtržítka
FLOATS = [
    ("0.0", "0.0"),
    ("3.14159", "3.14159"),
    ("1e10", "1e10"),
    ("2.5E-3", "2.5E-3"),
    ("100.5e+2", "100.5e+2"),
    ("1_000.50_1", "1_000.50_1"),
    ("42.0d", "42.0d"),
    ("1e5D", "1e5D"),
    ("0.5", "0.5"),
    ("5e-2", "5e-2")
]

# Řetězce – POUZE \\, \", \n, \r, \t! ŽÁDNÉ \x HEX ESCAPES!
STRINGS = [
    ('""', ''),
    ('"hello"', 'hello'),
    (r'"text s mezerami"', 'text s mezerami'),
    (r'"escape: \n nova radka"', 'escape: \n nova radka'),
    (r'"escape: \t tabulator"', 'escape: \t tabulator'),
    (r'"escape: \r carriage return"', 'escape: \r carriage return'),
    (r'"escape: \" uvozovka"', 'escape: " uvozovka'),
    (r'"escape: \\ lomitko"', 'escape: \\ lomitko'),
    (r'"kombinace: \n\t\"\\"', 'kombinace: \n\t"\\'),
    # Víceřádkový řetězec (Multiline string)
    ('"""line1\nline2\nline3"""', 'line1\nline2\nline3'),
    ('"""obsahuje \\n doslova bez escape"""', 'obsahuje \\n doslova bez escape')
]

# Platné operátory a oddělovače základního jazyka IFJ26 (bez složených závorek!)
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
    ("@", "AT"),      # pro anotaci @main
]

# Hraniční lexikální chyby (očekávaný návratový kód 1)
CRITICAL_LEX_ERRORS = [
    # Neplatné číselné literály
    ("err_num_leading_zero", "0123"),
    ("err_num_double_zero", "00"),
    ("err_num_trailing_underscore", "100_"),
    ("err_num_leading_underscore_in_digits", "1__0"),
    ("err_float_no_digits_after_dot", "42."),
    ("err_float_exp_missing_digits", "42e"),
    ("err_float_exp_sign_missing_digits", "42e+"),
    ("err_float_bad_suffix_f", "42.0f"),
    ("err_float_bad_suffix_F", "42.0F"),
    ("err_hex_number_unsupported", "0x12"),
    
    # Neplatné řetězce a escape sekvence (hex escape \x NENÍ v IFJ26 podporován)
    ("err_str_hex_escape_unsupported", r'"\x41"'),
    ("err_str_unknown_escape", r'"\a"'),
    ("err_str_unclosed_single_line", '"text bez uvozovky na konci'),
    ("err_str_raw_newline_in_single", '"prvni radek\ndruhy radek"'),
    ("err_multiline_str_unclosed", '"""neukonceny multiline'),
    
    # Neukončené komentáře
    ("err_comment_unclosed_at_eof", "/* neukonceny blokovy komentar"),
    ("err_comment_nested_unclosed", "/* prvni /* vnoreny */ neukonceny"),
    
    # Znaky a syntaxe zakázaná v IFJ26
    ("err_forbidden_lbrace", "{"),
    ("err_forbidden_rbrace", "}"),
    ("err_bad_tab_indent", "\tvar x = 1"),     # Tabulátor na začátku řádku je lex. chyba 1
    ("err_single_underscore_id", " _ "),      # Samotné podtržítko není platný identifikátor
    ("err_single_exclamation", "!"),          # Samotný vykřičník bez = (v základu bez BOOLTHEN)
    ("err_bad_char_hash", "#"),
    ("err_bad_char_dollar", "$"),
    ("err_bad_char_ampersand", "&"),
    ("err_bad_char_pipe", "|")
]

# ----------------------------------------------------------------------
# Zápis testovacích případů
# ----------------------------------------------------------------------

def write_case(target_dir: Path, name: str, src_content: str, expected_tokens: list[str], expected_exit: int = 0):
    target_dir.mkdir(parents=True, exist_ok=True)
    src_file = target_dir / f"{name}.ifj"
    exp_file = target_dir / f"{name}.expected"

    src_file.write_text(src_content, encoding="utf-8")
    with open(exp_file, "w", encoding="utf-8") as f:
        f.write(f"EXIT:{expected_exit}\n")
        for t in expected_tokens:
            f.write(f"{t}\n")

# ----------------------------------------------------------------------
# SADA: BASIC (Jednotkové testy pro sanity check)
# ----------------------------------------------------------------------

def gen_basic_suite(out_dir: Path):
    """Základní pokrytí klíčových slov, operátorů, literálů a komentářů."""
    for kw in KEYWORDS:
        write_case(out_dir, f"valid_kw_{kw}", kw, [f"1:KEYWORD:{kw}", "1:EOF"])

    for kw_ext in KEYWORDS_EXTENSIONS:
        write_case(out_dir, f"valid_kw_ext_{kw_ext}", kw_ext, [f"1:KEYWORD:{kw_ext}", "1:EOF"])

    for i, ident in enumerate(IDENTIFIERS):
        write_case(out_dir, f"valid_id_{i}", ident, [f"1:IDENTIFIER:{ident}", "1:EOF"])

    for i, val in enumerate(INTS):
        write_case(out_dir, f"valid_int_{i}", val, [f"1:LIT_INT:{val}", "1:EOF"])

    for i, (src, val) in enumerate(FLOATS):
        write_case(out_dir, f"valid_float_{i}", src, [f"1:LIT_FLOAT:{val}", "1:EOF"])

    for i, (src, val) in enumerate(STRINGS):
        write_case(out_dir, f"valid_str_{i}", src, [f"1:LIT_STRING:{val}", "1:EOF"])

    for op, tok in OPERATORS:
        write_case(out_dir, f"valid_op_{tok.lower()}", op, [f"1:{tok}", "1:EOF"])

    # Test na vnořený komentář (IFJ26 specifikum)
    write_case(out_dir, "valid_nested_comments", "/* vnejsi /* vnitrni */ vnejsi_konec */ 42", [
        "1:LIT_INT:42", "1:EOF"
    ])

    # Základní ungetc / nalepené tokeny bez mezer
    write_case(out_dir, "valid_tight_expr", "x=10+y.toStr()", [
        "1:IDENTIFIER:x", "1:ASSIGN", "1:LIT_INT:10", "1:PLUS",
        "1:IDENTIFIER:y", "1:DOT", "1:IDENTIFIER:toStr", "1:LPAREN", "1:RPAREN", "1:EOF"
    ])

    # Základní sadu chybových stavů
    for name, code in CRITICAL_LEX_ERRORS[:10]:
        write_case(out_dir, name, code, [], expected_exit=1)

# ----------------------------------------------------------------------
# SADA: ADVANCED (Zátěž, fuzzer a paměťový stres)
# ----------------------------------------------------------------------

def _pick_random_token(line: int) -> tuple[str, str, str]:
    category = random.choices(
        ["kw", "id", "int", "float", "str", "op"],
        weights=[15, 25, 20, 15, 10, 15]
    )[0]

    if category == "kw":
        val = random.choice(KEYWORDS + KEYWORDS_EXTENSIONS)
        return val, f"{line}:KEYWORD:{val}", category
    elif category == "id":
        val = random.choice(IDENTIFIERS)
        return val, f"{line}:IDENTIFIER:{val}", category
    elif category == "int":
        val = random.choice(INTS)
        return val, f"{line}:LIT_INT:{val}", category
    elif category == "float":
        val, repr_val = random.choice(FLOATS)
        return val, f"{line}:LIT_FLOAT:{repr_val}", category
    elif category == "str":
        # Pro náhodné baterie vynecháváme multiline s newliny, ať nerozhodíme počítadlo řádků
        val, repr_val = random.choice(STRINGS[:9])
        return val, f"{line}:LIT_STRING:{repr_val}", category
    else:
        val, tok_name = random.choice(OPERATORS)
        return val, f"{line}:{tok_name}", category


def gen_massive_battery(out_dir: Path, battery_id: int, target_tokens: int, inject_error: bool = False) -> int:
    random.seed(42000 + battery_id)

    source_parts = []
    expected_tokens = []
    line = 1

    error_idx = random.randint(target_tokens // 3, (2 * target_tokens) // 3) if inject_error else -1
    tokens_count = 0
    last_category = None

    for i in range(target_tokens):
        if inject_error and i == error_idx:
            err_name, err_payload = random.choice(CRITICAL_LEX_ERRORS)
            source_parts.append(f" {err_payload} ")
            break

        val, tok_repr, category = _pick_random_token(line)

        # Povinné mezery mezi alfanumerickými tokeny nebo sousedícími operátory
        need_mandatory_space = False
        if last_category:
            both_alphanumeric = (last_category in ["kw", "id", "int", "float"] and 
                                 category in ["kw", "id", "int", "float"])
            both_operators = (last_category == "op" and category == "op")
            if both_alphanumeric or both_operators:
                need_mandatory_space = True

        sep_type = random.choices(
            ["space", "newline", "line_comment", "block_comment", "none"],
            weights=[45, 20, 10, 10, 15]
        )[0]

        if need_mandatory_space and sep_type == "none":
            sep_type = "space"

        if sep_type == "space":
            source_parts.append(" " * random.randint(1, 3))
        elif sep_type == "newline":
            source_parts.append("\n")
            line += 1
        elif sep_type == "line_comment":
            source_parts.append(f" // comment on line {line}\n")
            line += 1
        elif sep_type == "block_comment":
            newlines = random.randint(0, 2)
            comment_newlines = "\n" * newlines
            source_parts.append(f" /* comment {comment_newlines} */ ")
            line += newlines
        elif sep_type == "none":
            source_parts.append("")

        source_parts.append(val)
        expected_tokens.append(tok_repr)
        tokens_count += 1

        last_category = category

    if not inject_error:
        expected_tokens.append(f"{line}:EOF")
        write_case(out_dir, f"battery_megamix_{battery_id}_{tokens_count}tok", "".join(source_parts), expected_tokens, expected_exit=0)
    else:
        write_case(out_dir, f"battery_needle_error_{battery_id}", "".join(source_parts), [], expected_exit=1)

    return tokens_count


def gen_advanced_suite(out_dir: Path):
    total_tokens = 0

    # 1. Čtyři velké baterie (2 000 až 3 000 tokenů)
    for b_id in range(1, 5):
        target = random.randint(2000, 3000)
        generated = gen_massive_battery(out_dir, b_id, target_tokens=target, inject_error=False)
        total_tokens += generated

    # 2. Deset středních baterií (po 400 tokenech)
    for b_id in range(10, 20):
        generated = gen_massive_battery(out_dir, b_id, target_tokens=400, inject_error=False)
        total_tokens += generated

    # 3. Pět baterií s jehlou v kupce sena (chyba uprostřed)
    for b_id in range(50, 55):
        gen_massive_battery(out_dir, b_id, target_tokens=500, inject_error=True)

    # 4. Paměťový stres pro dynamický buffer (dstring)
    huge_id = "var_" + ("a" * (128 * 1024))  # 128 KB identifikátor
    write_case(out_dir, "stress_128k_identifier", huge_id, [f"1:IDENTIFIER:{huge_id}", "1:EOF"])

    huge_str = "S" * (1024 * 1024)           # 1 MB string literál
    write_case(out_dir, "stress_1mb_string", f'"{huge_str}"', [f"1:LIT_STRING:{huge_str}", "1:EOF"])

    # 5. Zátěžový test pro vnořené blokové komentáře
    nesting = 100
    deep_comment = ("/* " * nesting) + "vnoreny text" + (" */" * nesting)
    write_case(out_dir, "stress_deep_nested_comments", f"val {deep_comment} x = 42", [
        "1:KEYWORD:val", "1:IDENTIFIER:x", "1:ASSIGN", "1:LIT_INT:42", "1:EOF"
    ])

    # 6. Komplet hraničních lexikálních chyb
    for name, code in CRITICAL_LEX_ERRORS:
        write_case(out_dir, f"edge_{name}", code, [], expected_exit=1)

    print(f"-> V Advanced sadě vygenerováno celkem ~{total_tokens} validních tokenů.")


def main():
    parser = argparse.ArgumentParser(description="IFJ26 Lexer Test Fixture Generator")
    parser.add_argument("--suite", choices=["basic", "advanced", "all"], default="all",
                        help="Kterou sadu vygenerovat (basic / advanced / all)")
    args = parser.parse_args()

    if args.suite in ["basic", "all"]:
        gen_basic_suite(BASE_FIXTURES / "basic")
        print("✓ Basic suite pro IFJ26 úspěšně vygenerována.")

    if args.suite in ["advanced", "all"]:
        gen_advanced_suite(BASE_FIXTURES / "advanced")
        print("✓ Advanced suite pro IFJ26 úspěšně vygenerována.")

if __name__ == "__main__":
    main()