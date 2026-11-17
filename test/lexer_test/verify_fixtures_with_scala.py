#!/usr/bin/env python3
import json
from pathlib import Path
import subprocess
import sys

ORACLE_SCALA_FILE = Path("./test/ScalaLexerOracle.scala")
FIXTURES_DIR = Path("./test/lexer_test/fixtures")

IFJ26_SPECIFIC_RESTRICTIONS = {
    "err_forbidden_lbrace",
    "err_forbidden_rbrace",
    "err_bad_char_hash",
    "err_bad_char_dollar",
    "err_bad_char_ampersand",
    "err_bad_char_backslash",
    "err_bad_char_caret",
    "err_bad_char_tilde",
    "err_bad_tab_indent",
    "err_bad_tab_after_spaces",
    "err_token_after_multiline_comment",
}

def run_batch_scala_lexer(files: list[Path], scala_bin: str) -> dict[str, tuple[bool, str]]:
    paths_payload = json.dumps([str(f.resolve()) for f in files])
    cmd = [
        scala_bin,
        "run",
        "--server=false",
        "--suppress-experimental-feature-warning",
        str(ORACLE_SCALA_FILE.resolve()),
    ]
    
    print(f"-> Spouštím Scala Scanner pro {len(files)} souborů...", flush=True)
    proc = subprocess.run(
        cmd,
        input=paths_payload,
        capture_output=True,
        text=True,
        timeout=180
    )

    if proc.returncode != 0:
        print(f"\n❌ Chyba při běhu Scala Scanneru (exit {proc.returncode}):\n{proc.stderr}", file=sys.stderr)
        sys.exit(1)

    lines = [line.strip() for line in proc.stdout.splitlines() if line.strip()]
    if not lines:
        print("\n❌ Scala nevrátila žádný výstup do stdout!", file=sys.stderr)
        if proc.stderr:
            print(f"Stderr:\n{proc.stderr}", file=sys.stderr)
        sys.exit(1)

    raw_json = lines[-1]
    try:
        data = json.loads(raw_json)
    except json.JSONDecodeError as e:
        print(f"\n❌ Selhalo parsování JSONu ze Scaly: {e}", file=sys.stderr)
        print(f"Surový výstup:\n{proc.stdout}", file=sys.stderr)
        sys.exit(1)

    return {item["path"]: (item["valid"], item["error"]) for item in data}

def main():
    scala_bin = "scala-cli"
    
    print(f"Hledám testovací soubory v: {FIXTURES_DIR.resolve()}...", flush=True)
    fixtures = sorted(FIXTURES_DIR.glob("**/*.ifj"))
    
    if not fixtures:
        print("❌ Nenalezeny žádné .ifj soubory! Spusťte nejdřív: python3 gen_fixtures.py --suite all", file=sys.stderr)
        sys.exit(1)

    print(f"Nalezeno {len(fixtures)} souborů. Zahajuji dávkovou kontrolu...", flush=True)
    batch_results = run_batch_scala_lexer(fixtures, scala_bin)

    mismatches = 0
    for f in fixtures:
        exp_file = f.with_suffix(".expected")
        if not exp_file.exists():
            print(f"⚠️ Chybí .expected pro: {f.name}")
            continue
            
        first_line = exp_file.read_text(encoding="utf-8").splitlines()[0]
        expected_exit = int(first_line.split(":")[1])
        valid, err_msg = batch_results.get(str(f.resolve()), (False, "Nenalezeno v orákulu"))

        # 1. Chyba: my čekáme EXIT 0, ale Scala vidí chybu
        if expected_exit == 0 and not valid:
            print(f"💥 KRITICKÝ ROZPOR (očekáván valid): {f.name}")
            print(f"   Scala Scanner odmítl: {err_msg}")
            mismatches += 1

        # 2. Chyba: my čekáme EXIT 1, Scala to bere jako validní a NENÍ to IFJ26 restrikce
        elif expected_exit == 1 and valid:
            is_known_restriction = any(r in f.name for r in IFJ26_SPECIFIC_RESTRICTIONS) or f.name.startswith("battery_needle_error_")
            if not is_known_restriction:
                print(f"💥 SPORNÝ NEGATIVNÍ TEST: {f.name}")
                print("   Očekáváme EXIT 1, ale Scala nehlásí žádnou lexikální chybu.")
                mismatches += 1

    print("\n" + "=" * 70)
    if mismatches == 0:
        print("✓ VŠECHNY TESTY JSOU V SOULADU S LEXEREM SCALY 3 A SPECIFIKACÍ IFJ26!")
    else:
        print(f"❌ Nalezeno {mismatches} rozporů.")
    print("=" * 70)

if __name__ == "__main__":
    main()