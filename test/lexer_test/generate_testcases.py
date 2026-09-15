#!/usr/bin/env python3
import os
import random
from pathlib import Path

OUT_DIR = Path("./test/lexer_test/fixtures")
OUT_DIR.mkdir(parents=True, exist_ok=True)

# ----------------------------------------------------------------------
# Definice stavebních kamenů podle očekávané podmnožiny Scaly
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
    (r'"text s mezera"', 'text s mezera'),
    (r'"escape: \n nova radka"', 'escape: \n nova radka'),
    (r'"escape: \t tabulator"', 'escape: \t tabulator'),
    (r'"escape: \" uvozovka"', 'escape: " uvozovka'),
    (r'"escape: \\ lomitko"', 'escape: \\ lomitko'),
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

WHITESPACES = [" ", "  ", "\t", "\n", " \t "]

# ----------------------------------------------------------------------
# Generátory testovacích souborů
# ----------------------------------------------------------------------

def write_case(name: str, src_content: str, expected_tokens: list[str], expected_exit: int = 0):
    src_file = OUT_DIR / f"{name}.ifj"
    exp_file = OUT_DIR / f"{name}.expected"
    
    with open(src_file, "w", encoding="utf-8") as f:
        f.write(src_content)
        
    with open(exp_file, "w", encoding="utf-8") as f:
        f.write(f"EXIT:{expected_exit}\n")
        for t in expected_tokens:
            f.write(f"{t}\n")

def gen_valid_cases():
    # 1. Základní tokeny samostatně
    for i, kw in enumerate(KEYWORDS):
        write_case(f"valid_kw_{kw}", kw, [f"1:KEYWORD:{kw}", "1:EOF"])

    for i, ident in enumerate(IDENTIFIERS):
        write_case(f"valid_id_{i}", ident, [f"1:IDENTIFIER:{ident}", "1:EOF"])

    for i, val in enumerate(INTS):
        write_case(f"valid_int_{i}", val, [f"1:LIT_INT:{val}", "1:EOF"])

    for i, (src, val) in enumerate(FLOATS):
        write_case(f"valid_float_{i}", src, [f"1:LIT_FLOAT:{val}", "1:EOF"])

    for i, (src, val) in enumerate(STRINGS):
        write_case(f"valid_str_{i}", src, [f"1:LIT_STRING:{val}", "1:EOF"])

    for op, tok in OPERATORS:
        safe_name = tok.lower()
        write_case(f"valid_op_{safe_name}", op, [f"1:{tok}", "1:EOF"])

    # 2. Hraniční případy s ungetc (žádné mezery mezi tokeny)
    ungetc_cases = [
        ("x=10", ["1:IDENTIFIER:x", "1:ASSIGN", "1:LIT_INT:10", "1:EOF"]),
        ("a+b*c", ["1:IDENTIFIER:a", "1:PLUS", "1:IDENTIFIER:b", "1:MUL", "1:IDENTIFIER:c", "1:EOF"]),
        ("12.5<=x", ["1:LIT_FLOAT:12.5", "1:LTE", "1:IDENTIFIER:x", "1:EOF"]),
        ("(x!=y)", ["1:LPAREN", "1:IDENTIFIER:x", "1:NEQ", "1:IDENTIFIER:y", "1:RPAREN", "1:EOF"]),
    ]
    for i, (code, toks) in enumerate(ungetc_cases):
        write_case(f"valid_tight_spacing_{i}", code, toks)

    # 3. Komentáře
    write_case(
        "valid_line_comment",
        "var x // komentar az do konce\nx = 1",
        ["1:KEYWORD:var", "1:IDENTIFIER:x", "2:IDENTIFIER:x", "2:ASSIGN", "2:LIT_INT:1", "2:EOF"]
    )

    write_case(
        "valid_block_comment",
        "val /* blokovy \n komentar */ a = 5",
        ["1:KEYWORD:val", "2:IDENTIFIER:a", "2:ASSIGN", "2:LIT_INT:5", "2:EOF"]
    )

    write_case(
        "valid_nested_comment",
        "a /* zanor /* druhy */ stale */ + b",
        ["1:IDENTIFIER:a", "1:PLUS", "1:IDENTIFIER:b", "1:EOF"]
    )

def gen_fuzzed_valid_sequences(count=15):
    """Náhodně vygeneruje delší platné sekvence tokenů oddělené náhodnými mezerami."""
    random.seed(42)
    
    for c in range(count):
        tokens_expected = []
        source_parts = []
        line = 1
        
        num_tokens = random.randint(10, 25)
        for _ in range(num_tokens):
            choice = random.choice(["kw", "id", "int", "op"])
            
            if choice == "kw":
                val = random.choice(KEYWORDS)
                source_parts.append(val)
                tokens_expected.append(f"{line}:KEYWORD:{val}")
                source_parts.append(" ")
            elif choice == "id":
                val = random.choice(IDENTIFIERS)
                source_parts.append(val)
                tokens_expected.append(f"{line}:IDENTIFIER:{val}")
                source_parts.append(" ")
            elif choice == "int":
                val = random.choice(INTS)
                source_parts.append(val)
                tokens_expected.append(f"{line}:LIT_INT:{val}")
                source_parts.append(" ")
            elif choice == "op":
                op_str, op_tok = random.choice(OPERATORS)
                source_parts.append(op_str)
                tokens_expected.append(f"{line}:{op_tok}")
                
                # Někdy dáme mezeru, někdy newline
                sep = random.choice(["", " ", "\n"])
                source_parts.append(sep)
                if sep == "\n":
                    line += 1

        tokens_expected.append(f"{line}:EOF")
        write_case(f"fuzzed_valid_{c}", "".join(source_parts), tokens_expected)

def gen_invalid_cases():
    """Vstupy, které MUSÍ skončit s chybou 1 (lexikální chyba)."""
    invalid = [
        ("lex_err_unclosed_str", '"text bez uvozovky na konci\n'),
        ("lex_err_unclosed_block_comment", '/* nezavreny komentar'),
        ("lex_err_single_exclamation", '!'),
        ("lex_err_bad_char_at", '@'),
        ("lex_err_bad_char_hash", '#'),
        ("lex_err_bad_char_dollar", '$'),
        ("lex_err_float_dot_alone", '12. var'),
        ("lex_err_float_exp_alone", '12e var'),
        ("lex_err_float_exp_sign_alone", '12e+ var'),
        ("lex_err_bad_escape", r'"spatny \q escape"'),
    ]
    for name, code in invalid:
        write_case(name, code, [], expected_exit=1)

if __name__ == "__main__":
    gen_valid_cases()
    gen_fuzzed_valid_sequences()
    gen_invalid_cases()
    total = len(list(OUT_DIR.glob("*.ifj")))
    print(f"Vygenerováno {total} testovacích případů do {OUT_DIR}/")