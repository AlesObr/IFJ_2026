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

# Celá čísla (Int) – v rámci 32-bit mezí pro plnou kompatibilitu se Scalou
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

# Desetinná čísla (Double)
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
    ("0.6", "0.6"),
    ("0.125e+2", "0.125e+2"),
    ("5e-2", "5e-2"),
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

# Skutečné lexikální chyby (návratový kód 1)
CRITICAL_LEX_ERRORS = [
    # Neplatná čísla / exponent
    ("err_float_exp_missing_digits", "42e"),
    # Neplatné řetězce a escape sekvence
    ("err_str_hex_escape_unsupported", r'"\x41"'),
    ("err_str_unknown_escape", r'"\a"'),
    ("err_str_unclosed_single_line", '"text bez uvozovky na konci'),
    ("err_str_raw_newline_in_single", '"prvni radek\ndruhy radek"'),
    ("err_multiline_str_unclosed", '"""neukonceny multiline'),
    # Komentáře
    ("err_comment_unclosed_at_eof", "/* neukonceny blokovy komentar"),
    ("err_comment_nested_unclosed", "/* prvni /* vnoreny */ neukonceny"),
    ("err_token_after_multiline_comment", "/* \n */ val x = 1"),
    # Zakázané neznámé znaky
    ("err_forbidden_lbrace", "{"),
    ("err_forbidden_rbrace", "}"),
    ("err_bad_char_hash", "#"),
    ("err_bad_char_dollar", "$"),
    ("err_bad_char_ampersand", "&"),
    ("err_bad_char_backslash", "\\"),
    ("err_bad_char_caret", "^"),
    ("err_bad_char_tilde", "~"),
]

# Poziční chyby odsazení (testovány izolovaně v basic suite)
INDENTATION_ERRORS = [
    ("err_bad_tab_indent", "\tvar x = 1"),
    ("err_bad_tab_after_spaces", "  \tvar x = 1"),
]

# ----------------------------------------------------------------------
# Zápis testovacích případů
# ----------------------------------------------------------------------


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
  """Základní jednotkové testy, rozpad sporných tokenů a ověření pravidel odsazení."""
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
    write_case(out_dir, f"valid_int_{i}", val, [f"1:LIT_INT:{val}", "1:EOF"])

  for i, (src, val) in enumerate(FLOATS):
    write_case(
        out_dir, f"valid_float_{i}", src, [f"1:LIT_FLOAT:{val}", "1:EOF"]
    )

  for i, (src, val) in enumerate(STRINGS):
    write_case(out_dir, f"valid_str_{i}", src, [f"1:LIT_STRING:{val}", "1:EOF"])

  for op, tok in OPERATORS:
    write_case(out_dir, f"valid_op_{tok.lower()}", op, [f"1:{tok}", "1:EOF"])

  # Desetinný literál začínající tečkou dle specifikace IFJ26
  write_case(
      out_dir, "valid_float_leading_dot", ".6", ["1:LIT_FLOAT:.6", "1:EOF"]
  )

  # Vnořené komentáře
  write_case(
      out_dir,
      "valid_nested_comments",
      "/* vnejsi /* vnitrni */ vnejsi_konec */ 42",
      ["1:LIT_INT:42", "1:EOF"],
  )

  # Výraz bez mezer (kontrola ungetc)
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

  # Případy rozpadající se na více tokenů (ne lexikální chyby)
  write_case(
      out_dir, "valid_unary_minus", "-42", ["1:MINUS", "1:LIT_INT:42", "1:EOF"]
  )
  write_case(
      out_dir,
      "valid_zero_and_ident",
      "0x12",
      ["1:LIT_INT:0", "1:IDENTIFIER:x12", "1:EOF"],
  )
  write_case(
      out_dir,
      "valid_float_split_f",
      "42.0f",
      ["1:LIT_FLOAT:42.0", "1:IDENTIFIER:f", "1:EOF"],
  )
  write_case(out_dir, "valid_isolated_dot", " . ", ["1:DOT", "1:EOF"])
  write_case(out_dir, "valid_underscore", " _ ", ["1:UNDERSCORE", "1:EOF"])

  # Platné odsazení mezerami
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
          "2:KEYWORD:val",
          "2:IDENTIFIER:x",
          "2:ASSIGN",
          "2:LIT_INT:1",
          "3:IDENTIFIER:x",
          "4:EOF",
      ],
  )

  # Zalomení řádku uvnitř závorek
  valid_parens_src = "val sum = (\n    10 +\n    20\n)\n"
  write_case(
      out_dir,
      "valid_parens_multiline",
      valid_parens_src,
      [
          "1:KEYWORD:val",
          "1:IDENTIFIER:sum",
          "1:ASSIGN",
          "1:LPAREN",
          "2:LIT_INT:10",
          "2:PLUS",
          "3:LIT_INT:20",
          "4:RPAREN",
          "5:EOF",
      ],
  )

  # Koncová čárka před RPAREN
  valid_trailing_comma_src = "foo(\n    1,\n    2,\n)\n"
  write_case(
      out_dir,
      "valid_trailing_comma",
      valid_trailing_comma_src,
      [
          "1:IDENTIFIER:foo",
          "1:LPAREN",
          "2:LIT_INT:1",
          "2:COMMA",
          "3:LIT_INT:2",
          "3:COMMA",
          "4:RPAREN",
          "5:EOF",
      ],
  )

  # Zápis všech ověřených chyb
  for name, code in CRITICAL_LEX_ERRORS + INDENTATION_ERRORS:
    write_case(out_dir, name, code, [], expected_exit=1)


# ----------------------------------------------------------------------
# SADA: ADVANCED & MASIVNÍ BATERIE
# ----------------------------------------------------------------------


def _pick_random_token_raw() -> tuple[str, str, str]:
  category = random.choices(
      ["kw", "id", "int", "float", "str", "op"],
      weights=[15, 25, 20, 15, 10, 15],
  )[0]

  if category == "kw":
    val = random.choice(KEYWORDS + KEYWORDS_EXTENSIONS)
    return val, f"KEYWORD:{val}", category
  elif category == "id":
    val = random.choice(IDENTIFIERS)
    return val, f"IDENTIFIER:{val}", category
  elif category == "int":
    val = random.choice(INTS)
    return val, f"LIT_INT:{val}", category
  elif category == "float":
    val, repr_val = random.choice(FLOATS)
    return val, f"LIT_FLOAT:{repr_val}", category
  elif category == "str":
    val, repr_val = random.choice(STRINGS[:9])
    return val, f"LIT_STRING:{repr_val}", category
  else:
    val, tok_name = random.choice(OPERATORS)
    return val, tok_name, category


def gen_massive_battery(
    out_dir: Path,
    battery_id: int,
    target_tokens: int,
    inject_error: bool = False,
) -> int:
  random.seed(42000 + battery_id)

  source_parts = []
  expected_tokens = []
  line = 1

  error_idx = (
      random.randint(target_tokens // 3, (2 * target_tokens) // 3)
      if inject_error
      else -1
  )
  tokens_count = 0

  for i in range(target_tokens):
    if inject_error and i == error_idx:
      err_name, err_payload = random.choice(CRITICAL_LEX_ERRORS)
      source_parts.append(f"\n{err_payload}\n")
      break

    val, tok_type_name, _ = _pick_random_token_raw()

    # V megamixu se tokeny oddělují mezerou, novým řádkem nebo komentářem
    sep_type = random.choices(
        ["space", "newline", "line_comment", "block_comment"],
        weights=[55, 25, 10, 10],
    )[0]

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
      if newlines > 0:
        comment_newlines = "\n" * newlines
        source_parts.append(f" /* comment {comment_newlines} */ \n")
        line += newlines + 1
      else:
        source_parts.append(" /* inline comment */ ")

    # Zápis tokenu
    source_parts.append(val)
    expected_tokens.append(f"{line}:{tok_type_name}")
    tokens_count += 1

  if not inject_error:
    expected_tokens.append(f"{line}:EOF")
    write_case(
        out_dir,
        f"battery_megamix_{battery_id}_{tokens_count}tok",
        "".join(source_parts),
        expected_tokens,
        expected_exit=0,
    )
  else:
    write_case(
        out_dir,
        f"battery_needle_error_{battery_id}",
        "".join(source_parts),
        [],
        expected_exit=1,
    )

  return tokens_count


def gen_advanced_suite(out_dir: Path):
  total_tokens = 0

  # Baterie bez chyb
  for b_id in range(1, 5):
    target = random.randint(2000, 3000)
    total_tokens += gen_massive_battery(
        out_dir, b_id, target_tokens=target, inject_error=False
    )

  for b_id in range(10, 20):
    total_tokens += gen_massive_battery(
        out_dir, b_id, target_tokens=400, inject_error=False
    )

  # Chybové jehly
  for b_id in range(50, 55):
    gen_massive_battery(out_dir, b_id, target_tokens=500, inject_error=True)

  # Paměťový stres
  huge_id = "var_" + ("a" * (128 * 1024))
  write_case(
      out_dir,
      "stress_128k_identifier",
      huge_id,
      [f"1:IDENTIFIER:{huge_id}", "1:EOF"],
  )

  huge_str = "S" * (1024 * 1024)
  write_case(
      out_dir,
      "stress_1mb_string",
      f'"{huge_str}"',
      [f"1:LIT_STRING:{huge_str}", "1:EOF"],
  )

  nesting = 100
  deep_comment = ("/* " * nesting) + "vnoreny text" + (" */" * nesting)
  write_case(
      out_dir,
      "stress_deep_nested_comments",
      f"val {deep_comment} x = 42",
      ["1:KEYWORD:val", "1:IDENTIFIER:x", "1:ASSIGN", "1:LIT_INT:42", "1:EOF"],
  )

  print(
      f"-> V Advanced sadě vygenerováno celkem ~{total_tokens} validních tokenů."
  )


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