#!/usr/bin/env python3
"""
Test runner pro IFJ26 parser a validaci AST S-výrazů.
Soubor byl vygenerován nástrojem generativní umělé inteligence (Gemini 3.8 flash)
"""

import argparse
import difflib
import subprocess
import sys
from pathlib import Path


def run_test(compiler_bin: str, scala_file: Path, check_ast: bool, use_valgrind: bool) -> tuple[bool, str]:
    cmd = []
    if use_valgrind:
        cmd.extend([
            "valgrind",
            "--leak-check=full",
            "--show-leak-kinds=all",
            "--error-exitcode=99",
            "-q"
        ])
    
    cmd.append(compiler_bin)
    if check_ast:
        cmd.append("--ast")

    is_error_test = "test_error" in scala_file.name

    try:
        with open(scala_file, "r", encoding="utf-8") as f_in:
            proc = subprocess.run(
                cmd,
                stdin=f_in,
                capture_output=True,
                text=True,
                timeout=4
            )
    except subprocess.TimeoutExpired:
        return False, "TIMEOUT (zacyklení)"
    except Exception as e:
        return False, f"Chyba při spuštění: {e}"

    if proc.returncode < 0:
        return False, f"CRASH: signál {-proc.returncode} (Segfault/Abort)"

    if use_valgrind and proc.returncode == 99:
        return False, f"VALGRIND CHYBA (Memory leak / invalid free):\n{proc.stderr.strip()}"

    # 1. Kontrola negativních testů (chyby syntaxe)
    if is_error_test:
        if proc.returncode in (1, 2):
            return True, f"OK (chyba zachycena, kód {proc.returncode})"
        return False, f"Chyba nezachycena! Očekáván kód 1 nebo 2, vráceno {proc.returncode}"

    # 2. Kontrola pozitivních testů (validní kód)
    if proc.returncode != 0:
        return False, f"Validní kód selhal s kódem {proc.returncode}! Stderr: {proc.stderr.strip()}"

    # 3. Kontrola AST struktury (pokud existuje referenční soubor .ast)
    ast_ref_file = scala_file.with_suffix(".ast")
    if check_ast and ast_ref_file.is_file():
        expected_ast = ast_ref_file.read_text(encoding="utf-8").strip()
        actual_ast = proc.stdout.strip()

        if expected_ast != actual_ast:
            diff = difflib.unified_diff(
                expected_ast.splitlines(),
                actual_ast.splitlines(),
                fromfile="expected.ast",
                tofile="actual.ast",
                lineterm=""
            )
            return False, "AST S-výraz neodpovídá:\n" + "\n".join(diff)

    return True, "OK"


def main():
    parser = argparse.ArgumentParser(description="Parser & AST Runner pro IFJ26")
    parser.add_argument("--bin", type=str, default="./parser_ast_test", help="Cesta k testovací binárce")
    parser.add_argument("--dir", type=str, default="generated_tests", help="Adresář s testy")
    parser.add_argument("--valgrind", action="store_true", help="Ověřovat paměť přes Valgrind")
    parser.add_argument("--check-ast", action="store_true", help="Porovnávat AST výstup s .ast soubory")
    args = parser.parse_args()

    bin_path = Path(args.bin)
    if not bin_path.is_file():
        print(f"❌ Binárka '{bin_path}' neexistuje. Nejprve sestavte parser test cíl.")
        sys.exit(1)

    tests = sorted(list(Path(args.dir).glob("*.scala")))
    if not tests:
        print(f"❌ Nenalezeny žádné soubory *.scala v '{args.dir}'.")
        sys.exit(1)

    print(f"🚀 Spouštím {len(tests)} testů (Valgrind: {'ZAPNUT' if args.valgrind else 'VYPNUT'}, AST kontrola: {'ZAPNUTA' if args.check_ast else 'VYPNUTA'})...\n")

    passed = 0
    failed = 0

    for test in tests:
        ok, msg = run_test(str(bin_path), test, check_ast=args.check_ast, use_valgrind=args.valgrind)
        if ok:
            passed += 1
            print(f"  [PASS] {test.name} -> {msg}")
        else:
            failed += 1
            print(f"❌ [FAIL] {test.name}\n      {msg}")

    print("\n" + "=" * 50)
    print(f"Celkem: {passed} prošlo, {failed} selhalo z {len(tests)} testů.")
    if failed > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()