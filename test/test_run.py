#!/usr/bin/env python3
# Test runner pro lexer
# Author: Aleš Obr xobrale00
import sys
import subprocess
import difflib
from pathlib import Path

# File paths
REPO_ROOT = Path(__file__).resolve().parent.parent
FIXTURES_DIR = REPO_ROOT / "test" / "lexer_test" / "fixtures" 
BINARY = REPO_ROOT / "build" / "lexer_test"

GREEN = "\033[92m"
RED = "\033[91m"
YELLOW = "\033[93m"
BOLD = "\033[1m"
RESET = "\033[0m"

def run_single_test(test_file: Path, use_valgrind: bool = False):
    exp_file = test_file.with_suffix(".expected")
    if not exp_file.exists():
        return False, f"Chybí referenční soubor: {exp_file.name}"

    with open(exp_file, "r", encoding="utf-8") as f:
        exp_lines = [line.strip() for line in f if line.strip()]

    if not exp_lines or not exp_lines[0].startswith("EXIT:"):
        return False, f"Chybný formát souboru {exp_file.name}: chybí hlavička EXIT:kód"

    expected_exit = int(exp_lines[0].split(":")[1])
    expected_tokens = exp_lines[1:]

    cmd = []
    if use_valgrind:
        cmd.extend([
            "valgrind",
            "--leak-check=full",
            "--error-exitcode=99",
            "-q"
        ])
    cmd.append(str(BINARY))

    with open(test_file, "r", encoding="utf-8") as src_in:
        proc = subprocess.run(
            cmd,
            stdin=src_in,
            text=True,
            capture_output=True
        )

    if use_valgrind and proc.returncode == 99:
        return False, f"Valgrind Memory Leak / Error:\n{proc.stderr}"

    if proc.returncode != expected_exit:
        return False, (
            f"Očekáván exit kód {expected_exit}, ale vráceno {proc.returncode}.\n"
            f"STDERR:\n{proc.stderr.strip()}"
        )

    if expected_exit == 0:
        actual_tokens = [line.strip() for line in proc.stdout.splitlines() if line.strip()]
        if actual_tokens != expected_tokens:
            diff = difflib.unified_diff(
                expected_tokens,
                actual_tokens,
                fromfile="expected",
                tofile="actual",
                lineterm=""
            )
            return False, "Rozdíl v tokenech:\n" + "\n".join(diff)

    return True, ""

def main():
    if not BINARY.exists():
        print(f"{RED}{BOLD}Chyba: Testovací binárka neexistuje na {BINARY}!{RESET}")
        print("Spusť nejdřív: make scanner-build")
        sys.exit(1)

    use_valgrind = "--valgrind" in sys.argv
    # Rekurzivní vyhledání testů ve všech podsložkách (basic, advanced...)
    test_files = sorted(list(FIXTURES_DIR.rglob("*.ifj")))

    if not test_files:
        print(f"{YELLOW}Ve složce {FIXTURES_DIR} nebyly nalezeny žádné .ifj testy.{RESET}")
        print("Spusť nejprve: python3 test/lexer_test/generate_testcases.py")
        sys.exit(1)

    print(f"{BOLD}Spouštím {len(test_files)} testů lexeru...{RESET}")
    if use_valgrind:
        print(f"{YELLOW}Aktivní kontrola přes Valgrind{RESET}")

    passed = 0
    failed = 0

    for test_path in test_files:
        test_display = test_path.relative_to(FIXTURES_DIR)
        ok, msg = run_single_test(test_path, use_valgrind)

        if ok:
            passed += 1
            print(f"  [{GREEN}PASS{RESET}] {test_display}")
        else:
            failed += 1
            print(f"  [{RED}FAIL{RESET}] {test_display}")
            for line in msg.splitlines():
                print(f"         {line}")

    print("\n" + "=" * 50)
    if failed == 0:
        print(f"{GREEN}{BOLD}VŠECHNY TESTY PROŠLY ({passed}/{len(test_files)}){RESET}")
    else:
        print(f"{RED}{BOLD}SELHALO {failed} Z {len(test_files)} TESTŮ{RESET}")
        sys.exit(1)

if __name__ == "__main__":
    main()