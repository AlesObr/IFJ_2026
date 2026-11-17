#!/usr/bin/env python3
"""
Test runner pro IFJ26 parser a validaci AST S-výrazů.
Soubor byl vygenerován nástrojem generativní umělé inteligence (Gemini 3.8 flash)
"""

import argparse
import difflib
import re
import subprocess
import sys
from pathlib import Path


AST_MATCH_MESSAGE = "OK (AST odpovídá referenci)"
POSITION_PATTERN = re.compile(r"\bat (\d+:\d+)")
# Kód 99 je ERR_INTERNAL překladače, Valgrind proto musí hlásit chybu jiným kódem
VALGRIND_EXIT_CODE = 77
TIMEOUT_SECONDS = 4
# Valgrind zpomaluje běh zhruba desetkrát (část testů běží pod ním přes 4 s)
VALGRIND_TIMEOUT_SECONDS = 60


def format_stats(passed: int, failed: int) -> str:
    total = passed + failed
    percent = 100 * passed / total if total else 0.0
    return f"{passed} prošlo, {failed} selhalo z {total} testů ({percent:.1f} %)."


def last_dump_line(stderr: str) -> str:
    """Returns the last dump line; parser messages end with \\n, leaving a lone quote after them."""
    lines = [line.strip() for line in stderr.splitlines() if line.strip() not in ("", '"')]
    return lines[-1] if lines else "(prázdný výpis)"


def print_failure_analysis(failures: dict[str, list[tuple[str, str]]], total_failed: int) -> None:
    """Prints failure counts by kind, with the grouped last dump lines under each."""
    print(f"\nAnalýza selhání ({total_failed} testů):")
    for kind, items in sorted(failures.items(), key=lambda item: -len(item[1])):
        print(f"  {len(items)}x ({100 * len(items) / total_failed:.1f} %) {kind}")

        # The position differs between tests, so grouping ignores it
        groups: dict[str, list[tuple[str, str]]] = {}
        for test_name, dump in items:
            if dump:
                key = POSITION_PATTERN.sub("at L:S", dump)
                groups.setdefault(key, []).append((test_name, dump))

        for key, group in sorted(groups.items(), key=lambda item: -len(item[1])):
            test_name, dump = group[0]
            position = POSITION_PATTERN.search(dump)
            where = f" ({position.group(1)})" if position else ""
            print(f"      {len(group)}x {key}")
            print(f"          např. {test_name}{where}")


def run_test(compiler_bin: str, scala_file: Path, check_ast: bool, use_valgrind: bool) -> tuple[bool, str, str | None, str]:
    """Returns (passed, message, failure kind, last dump line for the analysis)."""
    cmd = []
    if use_valgrind:
        cmd.extend([
            "valgrind",
            "--leak-check=full",
            "--show-leak-kinds=all",
            f"--error-exitcode={VALGRIND_EXIT_CODE}",
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
                timeout=VALGRIND_TIMEOUT_SECONDS if use_valgrind else TIMEOUT_SECONDS
            )
    except subprocess.TimeoutExpired:
        return False, "TIMEOUT (zacyklení)", "Timeout (zacyklení)", ""
    except Exception as e:
        return False, f"Chyba při spuštění: {e}", "Chyba při spuštění", ""

    # Exit code 0 means no error, so its last dump line carries no information
    dump = last_dump_line(proc.stderr) if proc.returncode != 0 else ""

    if proc.returncode < 0:
        return False, f"CRASH: signál {-proc.returncode} (Segfault/Abort)", f"Crash (signál {-proc.returncode})", dump

    if use_valgrind and proc.returncode == VALGRIND_EXIT_CODE:
        return False, f"VALGRIND CHYBA (Memory leak / invalid free):\n{proc.stderr.strip()}", f"Valgrind (kód {VALGRIND_EXIT_CODE})", ""

    # 1. Kontrola negativních testů (chyby syntaxe)
    if is_error_test:
        if proc.returncode in (1, 2):
            return True, f"OK (chyba zachycena, kód {proc.returncode})", None, ""
        return (
            False,
            f"Chyba nezachycena! Očekáván kód 1 nebo 2, vráceno {proc.returncode}",
            f"Chybový test: špatný kód {proc.returncode} (očekáván 1 nebo 2)",
            dump,
        )

    # 2. Kontrola pozitivních testů (validní kód)
    if proc.returncode != 0:
        return (
            False,
            f"Validní kód selhal s kódem {proc.returncode}! Stderr: {proc.stderr.strip()}",
            f"Nechybový test: špatný kód {proc.returncode} (očekávána 0)",
            dump,
        )

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
            return False, "AST S-výraz neodpovídá:\n" + "\n".join(diff), "Chyba AST (neshoda s referencí)", ""
        return True, AST_MATCH_MESSAGE, None, ""

    return True, "OK", None, ""


def main():
    parser = argparse.ArgumentParser(description="Parser & AST Runner pro IFJ26")
    parser.add_argument("--bin", type=str, default="./parser_ast_test", help="Cesta k testovací binárce")
    parser.add_argument("--dir", type=str, default="generated_tests", help="Adresář s testy")
    parser.add_argument("--valgrind", action="store_true", help="Ověřovat paměť přes Valgrind")
    parser.add_argument("--check-ast", action="store_true", help="Porovnávat AST výstup s .ast soubory")
    parser.add_argument("--repeat", type=int, default=1,
                        help="Spustit celou sadu N× a vypsat jen souhrn a testy, které se mezi běhy lišily")
    args = parser.parse_args()

    if args.repeat < 1:
        print("❌ --repeat musí být alespoň 1.")
        sys.exit(1)

    bin_path = Path(args.bin)
    if not bin_path.is_file():
        print(f"❌ Binárka '{bin_path}' neexistuje. Nejprve sestavte parser test cíl.")
        sys.exit(1)

    tests = sorted(list(Path(args.dir).glob("*.scala")))
    if not tests:
        print(f"❌ Nenalezeny žádné soubory *.scala v '{args.dir}'.")
        sys.exit(1)

    repeat_info = f", opakování: {args.repeat}×" if args.repeat > 1 else ""
    print(f"🚀 Spouštím {len(tests)} testů (Valgrind: {'ZAPNUT' if args.valgrind else 'VYPNUT'}, AST kontrola: {'ZAPNUTA' if args.check_ast else 'VYPNUTA'}{repeat_info})...\n")

    # Absolute path: Path("./bin") drops "./" and the binary would be looked up in PATH
    compiler_bin = str(bin_path.resolve())
    runs: list[dict[str, TestResult]] = []
    for run_index in range(args.repeat):
        # Výpis jednotlivých testů jen při jediném běhu, jinak by byl N× delší
        results = run_suite(compiler_bin, tests, args, verbose=args.repeat == 1)
        runs.append(results)
        if args.repeat > 1:
            run_failed = sum(not r.ok for r in results.values())
            print(f"Běh {run_index + 1}/{args.repeat}: {format_stats(len(results) - run_failed, run_failed)}")

    print("\n" + "=" * 50)
    if args.repeat > 1:
        print_run_differences(runs)
        print("Souhrn prvního běhu:")
    failed = print_summary(runs[0], args.check_ast)
    if failed > 0 or any(not r.ok for results in runs for r in results.values()):
        sys.exit(1)


class TestResult:
    """Výsledek jednoho spuštění testu."""

    def __init__(self, test_name: str, ok: bool, msg: str, kind: str | None, dump: str):
        self.test_name = test_name
        self.ok = ok
        self.msg = msg
        self.kind = kind
        self.dump = dump

    def outcome(self) -> str:
        """Krátký popis výsledku pro porovnání běhů (stejný popis = stejný výsledek)."""
        return "PASS" if self.ok else f"FAIL: {self.kind}"


def run_suite(compiler_bin: str, tests: list[Path], args: argparse.Namespace, verbose: bool) -> dict[str, TestResult]:
    """Spustí všechny testy jednou; při verbose vypisuje každý test."""
    results: dict[str, TestResult] = {}
    for test in tests:
        ok, msg, kind, dump = run_test(compiler_bin, test, check_ast=args.check_ast, use_valgrind=args.valgrind)
        results[test.name] = TestResult(test.name, ok, msg, kind, dump)
        if verbose:
            if ok:
                print(f"✅  [PASS] {test.name} -> {msg}")
            else:
                print(f"❌ [FAIL] {test.name}\n      {msg}")
    return results


def print_summary(results: dict[str, TestResult], check_ast: bool) -> int:
    """Vypíše statistiku a analýzu selhání jednoho běhu; vrátí počet selhání."""
    passed = failed = ast_matched = 0
    # Category -> [passed, failed]
    stats = {"Nechybové": [0, 0], "Chybové": [0, 0]}
    # Failure kind -> [(test name, last dump line)]
    failures: dict[str, list[tuple[str, str]]] = {}
    for result in results.values():
        category = "Chybové" if "test_error" in result.test_name else "Nechybové"
        stats[category][0 if result.ok else 1] += 1
        if result.ok:
            passed += 1
            ast_matched += result.msg == AST_MATCH_MESSAGE
        else:
            failed += 1
            failures.setdefault(result.kind, []).append((result.test_name, result.dump))

    for category, (category_passed, category_failed) in stats.items():
        print(f"{category}: {format_stats(category_passed, category_failed)}")
    print(f"Celkem: {format_stats(passed, failed)}")
    if check_ast:
        print(f"AST: {ast_matched} testů prošlo porovnáním s referencí (ostatní .ast nemají).")
    if failed > 0:
        print_failure_analysis(failures, failed)
    return failed


def print_run_differences(runs: list[dict[str, TestResult]]) -> None:
    """Vypíše testy, jejichž výsledek se mezi běhy lišil."""
    unstable = []
    for test_name in runs[0]:
        outcomes = [results[test_name].outcome() for results in runs]
        if len(set(outcomes)) > 1:
            unstable.append((test_name, outcomes))

    if not unstable:
        print(f"✅ Všech {len(runs)} běhů dopadlo u každého testu stejně.\n")
        return

    print(f"⚠️  Výsledek se mezi {len(runs)} běhy lišil u {len(unstable)} testů:")
    for test_name, outcomes in unstable:
        counts: dict[str, int] = {}
        for outcome in outcomes:
            counts[outcome] = counts.get(outcome, 0) + 1
        summary = ", ".join(f"{outcome} {count}×" for outcome, count in counts.items())
        print(f"  {test_name}: {summary}")
    print()


if __name__ == "__main__":
    main()