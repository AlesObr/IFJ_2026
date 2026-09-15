#!/usr/bin/env python3
"""
Generátor deterministických a zátěžových testů pro IFJ25 Lexer.
Generuje sadu 'basic' pro rychlý lokální vývoj a sadu 'advanced'
(vyšší tisíce tokenů, stres dynamické paměti a zákeřné rohy) pro CI/CD pipeline.
"""

import argparse
import random
from pathlib import Path

BASE_FIXTURES = Path("./test/lexer_test/fixtures")

# ----------------------------------------------------------------------
# Definice stavebních kamenů podle specifikace
# ----------------------------------------------------------------------

KEYWORDS = ["def", "var", "val", "if", "else", "while", "return"]

IDENTIFIERS = [
    "x", "_var", "camelCase", "snake_case", "with123Numbers",
    "_1", "__private", "veryLongIdentifierNameThatShouldStillWorkInDynamicString"
]

INTS = ["0", "1", "42", "999999999", "1234567890123456"]

FLOATS = [
    ("0.0", "0.0"),
    ("3.14159", "3.14159"),
    ("1e10", "1e10"),
    ("2.5E-3", "2.5E-3"),
    ("100.5e+2", "100.5e+2"),
]

# (raw_c_string_in_file, parsed_value_expected)
STRINGS = [
    ('""', ''),
    ('"hello"', 'hello'),
    (r'"text s mezerami"', 'text s mezerami'),
    (r'"escape: \n nova radka"', 'escape: \n nova radka'),
    (r'"escape: \t tabulator"', 'escape: \t tabulator'),
    (r'"escape: \" uvozovka"', 'escape: " uvozovka'),
    (r'"escape: \\ lomitko"', 'escape: \\ lomitko'),
    (r'"hex: \x41"', 'hex: A'),
    (r'"hex male: \x61"', 'hex male: a'),
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
    ("{", "LBRACE"),
    ("}", "RBRACE"),
    (":", "COLON"),
    (";", "SEMICOLON"),
    (",", "COMMA"),
]

# Hraniční chyby cílené na exit code 1 (Lexikální chyba)
CRITICAL_LEX_ERRORS = [
    ("err_num_leading_zero", "0123"),
    ("err_num_double_zero", "00"),
    ("err_float_no_digits_after_dot", "42."),
    ("err_float_exp_missing_digits", "42e"),
    ("err_float_exp_sign_missing_digits", "42e+"),
    ("err_hex_no_digits", "0x"),
    ("err_hex_invalid_char", "0xZ"),
    ("err_str_incomplete_hex_escape", r'"\xA"'),
    ("err_str_bad_hex_escape", r'"\xZZ"'),
    ("err_str_unclosed_at_eof", '"text bez uvozovky na konci'),
    ("err_str_raw_newline", '"prvni radek\ndruhy radek"'),
    ("err_comment_unclosed_at_eof", "/* neukonceny blokovy komentar"),
    ("err_single_exclamation", "!"),
    ("err_bad_char_at", "@"),
    ("err_bad_char_hash", "#"),
    ("err_bad_char_dollar", "$"),
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
# SADA: BASIC (Rychlý lokální testovací běh)
# ----------------------------------------------------------------------

def gen_basic_suite(out_dir: Path):
    """Základní pokrytí všech kategorií po 1 kusu pro rychlý sanity check."""
    for kw in KEYWORDS:
        write_case(out_dir, f"valid_kw_{kw}", kw, [f"1:KEYWORD:{kw}", "1:EOF"])

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

    # Základní ungetc bez mezer
    write_case(out_dir, "valid_tight_expr", "x=10+y", [
        "1:IDENTIFIER:x", "1:ASSIGN", "1:LIT_INT:10", "1:PLUS", "1:IDENTIFIER:y", "1:EOF"
    ])

    # Základní chyby
    for name, code in CRITICAL_LEX_ERRORS[:6]:
        write_case(out_dir, name, code, [], expected_exit=1)

# ----------------------------------------------------------------------
# SADA: ADVANCED (Zátěž vyšších tisíců tokenů, megamixy a fuzzing)
# ----------------------------------------------------------------------

def _pick_random_token(line: int) -> tuple[str, str, str]:
    """Vrátí (text_do_zdroje, ocekavany_token_radek, kategorie)."""
    category = random.choices(
        ["kw", "id", "int", "float", "str", "op"],
        weights=[15, 25, 20, 15, 10, 15]
    )[0]

    if category == "kw":
        val = random.choice(KEYWORDS)
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
        val, repr_val = random.choice(STRINGS)
        return val, f"{line}:LIT_STRING:{repr_val}", category
    else:
        val, tok_name = random.choice(OPERATORS)
        return val, f"{line}:{tok_name}", category


def gen_massive_battery(out_dir: Path, battery_id: int, target_tokens: int, inject_error: bool = False) -> int:
    """
    Sestaví jednu masivní baterii tokenů (stovky až tisíce v jednom souboru).
    Bezpečně ošetřuje oddělovače, aby nedocházelo k nechtěnému splynutí lexémů.
    Vrací skutečný počet vygenerovaných tokenů.
    """
    random.seed(42000 + battery_id)

    source_parts = []
    expected_tokens = []
    line = 1

    error_idx = random.randint(target_tokens // 3, (2 * target_tokens) // 3) if inject_error else -1
    tokens_count = 0
    last_category = None
    last_val = ""

    for i in range(target_tokens):
        if inject_error and i == error_idx:
            # Vstříknutí náhodné kritické chyby
            err_name, err_payload = random.choice(CRITICAL_LEX_ERRORS)
            source_parts.append(f" {err_payload} ")
            # Lexer tady musí spadnout s exit 1, další tokeny se netestují
            break

        val, tok_repr, category = _pick_random_token(line)

        # Bezpečné oddělení proti nechtěnému splynutí
        # (např. 'var' + 'x' -> 'varx', nebo '/' + '/' -> '//', nebo '=' + '=' -> '==')
        need_mandatory_space = False
        if last_category:
            both_alphanumeric = (last_category in ["kw", "id", "int", "float"] and 
                                 category in ["kw", "id", "int", "float"])
            both_operators = (last_category == "op" and category == "op")
            if both_alphanumeric or both_operators:
                need_mandatory_space = True

        sep_type = random.choices(
            ["space", "newline", "line_comment", "block_comment", "none"],
            weights=[40, 25, 10, 10, 15]
        )[0]

        if need_mandatory_space and sep_type == "none":
            sep_type = "space"

        # Přidání oddělovače
        if sep_type == "space":
            source_parts.append(" " * random.randint(1, 3))
        elif sep_type == "newline":
            source_parts.append("\n")
            line += 1
        elif sep_type == "line_comment":
            source_parts.append(f" // comment on line {line}\n")
            line += 1
        elif sep_type == "block_comment":
            newlines = random.randint(0, 3)
            newlines = random.randint(0, 3)
            comment_newlines = "\n" * newlines
            source_parts.append(f" /* comment {comment_newlines} */ ")
            line += newlines
        elif sep_type == "none":
            source_parts.append("")

        source_parts.append(val)
        expected_tokens.append(tok_repr)
        tokens_count += 1

        last_category = category
        last_val = val

    if not inject_error:
        expected_tokens.append(f"{line}:EOF")
        write_case(out_dir, f"battery_megamix_{battery_id}_{tokens_count}tok", "".join(source_parts), expected_tokens, expected_exit=0)
    else:
        write_case(out_dir, f"battery_needle_error_{battery_id}", "".join(source_parts), [], expected_exit=1)

    return tokens_count


def gen_advanced_suite(out_dir: Path):
    """Vygeneruje vyšší tisíce tokenů rozdělených do validních i chybových baterií."""
    total_tokens = 0

    # 1. Čtyři gigantické validní baterie (každá 2 000 až 3 000 tokenů) -> cca 10 000 tokenů
    for b_id in range(1, 5):
        target = random.randint(2000, 3000)
        generated = gen_massive_battery(out_dir, b_id, target_tokens=target, inject_error=False)
        total_tokens += generated

    # 2. Deset středních baterií (po 400 tokenech) -> cca 4 000 tokenů
    for b_id in range(10, 20):
        generated = gen_massive_battery(out_dir, b_id, target_tokens=400, inject_error=False)
        total_tokens += generated

    # 3. Pět baterií s "jehlou v kupce sena" (stovky tokenů a pak skrytý pád na exit 1)
    for b_id in range(50, 55):
        gen_massive_battery(out_dir, b_id, target_tokens=500, inject_error=True)

    # 4. Paměťový stres pro dynamický buffer (dstring)
    huge_id = "var_" + ("a" * (128 * 1024)) # 128 KB identifikátor
    write_case(out_dir, "stress_128k_identifier", huge_id, [f"1:IDENTIFIER:{huge_id}", "1:EOF"])

    huge_str = "S" * (1024 * 1024) # 1 MB string literál
    write_case(out_dir, "stress_1mb_string", f'"{huge_str}"', [f"1:LIT_STRING:{huge_str}", "1:EOF"])

    # 5. Extrémní hloubka vnoření komentářů (test zanořování / čítače)
    nesting = 80
    deep_comment = ("/* " * nesting) + "hluboko" + (" */" * nesting)
    write_case(out_dir, "stress_deep_nested_comments", f"var {deep_comment} x = 42", [
        "1:KEYWORD:var", "1:IDENTIFIER:x", "1:ASSIGN", "1:LIT_INT:42", "1:EOF"
    ])

    # 6. Všechny hraniční chyby
    for name, code in CRITICAL_LEX_ERRORS:
        write_case(out_dir, f"edge_{name}", code, [], expected_exit=1)

    print(f"-> V Advanced sadě vygenerováno celkem ~{total_tokens} validních tokenů v bateriích.")


def main():
    parser = argparse.ArgumentParser(description="IFJ Lexer Test Fixture Generator")
    parser.add_argument("--suite", choices=["basic", "advanced", "all"], default="all",
                        help="Kterou sadu vygenerovat (basic / advanced / all)")
    args = parser.parse_args()

    if args.suite in ["basic", "all"]:
        gen_basic_suite(BASE_FIXTURES / "basic")
        print("✓ Basic suite vygenerována.")

    if args.suite in ["advanced", "all"]:
        gen_advanced_suite(BASE_FIXTURES / "advanced")
        print("✓ Advanced suite vygenerována.")

if __name__ == "__main__":
    main()