#!/usr/bin/env python3
"""
Test runner semanticke analyzy IFJ26.

Ocekavany navratovy kod je dany nazvem testu: ok_* -> 0, eN_* -> N (napr. e6_decl_type_x.scala
ceka chybu 6). Runner vypise statistiku po ocekavanych kodech a analyzu selhani: ocekavany vs.
skutecny kod a posledni radek vypisu (zpravu analyzatoru), seskupene podle zpravy.

Pouziti: python3 sem_runner.py --bin ../../build/semantic_test --dir cases generated [--valgrind] [-v]
"""

import argparse
import re
import subprocess
import sys
from pathlib import Path

EXPECTED_PATTERN = re.compile(r"^(?:ok_|e(\d+)_)")
POSITION_PATTERN = re.compile(r"\b(\d+:\d+)\b")
# 99 je ERR_INTERNAL prekladace, Valgrind proto musi hlasit chybu jinym kodem
VALGRIND_EXIT_CODE = 77
TIMEOUT_SECONDS = 4
VALGRIND_TIMEOUT_SECONDS = 60

CODE_NAMES = {
    0: "bez chyby", 1: "lexikalni", 2: "syntakticka", 3: "nedefinovany symbol", 4: "redefinice",
    5: "volani / argumenty", 6: "typova kompatibilita", 7: "zapis do nemodifikovatelne",
    10: "ostatni semanticke", 99: "interni",
}


def expected_code(test: Path) -> int | None:
    match = EXPECTED_PATTERN.match(test.name)
    if match is None:
        return None
    return int(match.group(1)) if match.group(1) else 0


def last_message(stderr: str) -> str:
    lines = [line.strip() for line in stderr.splitlines() if line.strip() not in ("", '"')]
    semantic = [line for line in lines if line.startswith("[semantic]")]
    return (semantic or lines or ["(prazdny vypis)"])[-1]


def run_test(binary: str, test: Path, use_valgrind: bool) -> tuple[int | None, str]:
    """Returns (exit code or None on timeout/crash, last message)."""
    cmd = ["valgrind", "-q", f"--error-exitcode={VALGRIND_EXIT_CODE}", "--leak-check=no"] if use_valgrind else []
    cmd.append(binary)
    try:
        with open(test, encoding="utf-8") as f:
            proc = subprocess.run(cmd, stdin=f, capture_output=True, text=True,
                                  timeout=VALGRIND_TIMEOUT_SECONDS if use_valgrind else TIMEOUT_SECONDS)
    except subprocess.TimeoutExpired:
        return None, "TIMEOUT (zacykleni)"
    if proc.returncode < 0:
        return None, f"CRASH: signal {-proc.returncode}"
    if use_valgrind and proc.returncode == VALGRIND_EXIT_CODE:
        return None, "VALGRIND: neplatny pristup do pameti"
    return proc.returncode, last_message(proc.stderr)


def format_stats(passed: int, failed: int) -> str:
    total = passed + failed
    percent = 100 * passed / total if total else 0.0
    return f"{passed} proslo, {failed} selhalo z {total} testu ({percent:.1f} %)."


def code_label(code: int | None) -> str:
    return "pad/timeout" if code is None else f"{code} ({CODE_NAMES.get(code, '?')})"


def main():
    parser = argparse.ArgumentParser(description="Semantic runner pro IFJ26")
    parser.add_argument("--bin", default="./semantic_test", help="Testovaci binarka (parser + semantika)")
    parser.add_argument("--dir", nargs="+", default=["cases"], help="Adresare s testy *.scala")
    parser.add_argument("--valgrind", action="store_true", help="Spoustet pod Valgrindem (jen neplatne pristupy)")
    parser.add_argument("-v", "--verbose", action="store_true", help="Vypsat vysledek kazdeho testu")
    parser.add_argument("--filter", default=None, help="Spustit jen testy, jejichz nazev obsahuje tento text")
    args = parser.parse_args()

    binary = Path(args.bin)
    if not binary.is_file():
        print(f"Binarka '{binary}' neexistuje. Nejprve sestavte cil semantic-build.")
        sys.exit(1)
    tests = sorted(t for d in args.dir for t in Path(d).glob("*.scala")
                   if expected_code(t) is not None and (args.filter is None or args.filter in t.name))
    if not tests:
        print(f"Nenalezeny zadne testy ok_*/eN_* v {args.dir}.")
        sys.exit(1)

    print(f"Spoustim {len(tests)} testu (Valgrind: {'ZAPNUT' if args.valgrind else 'VYPNUT'})...\n")
    # expected code -> [passed, failed]
    stats: dict[int, list[int]] = {}
    # (expected, actual) -> [(test, message)]
    failures: dict[tuple[int, int | None], list[tuple[str, str]]] = {}
    for test in tests:
        expected = expected_code(test)
        actual, message = run_test(str(binary.resolve()), test, args.valgrind)
        ok = actual == expected
        stats.setdefault(expected, [0, 0])[0 if ok else 1] += 1
        if not ok:
            failures.setdefault((expected, actual), []).append((test.name, message))
        if args.verbose:
            mark = "PASS" if ok else "FAIL"
            detail = "" if ok else f" (ocekavan {expected}, vraceno {actual}: {message})"
            print(f"[{mark}] {test.parent.name}/{test.name}{detail}")

    print("=" * 60)
    passed = sum(s[0] for s in stats.values())
    failed = sum(s[1] for s in stats.values())
    for code in sorted(stats):
        print(f"Ocekavan {code_label(code)}: {format_stats(*stats[code])}")
    print(f"Celkem: {format_stats(passed, failed)}")

    if failures:
        print(f"\nAnalyza selhani ({failed} testu):")
        for (expected, actual), items in sorted(failures.items(), key=lambda item: -len(item[1])):
            print(f"  {len(items)}x ocekavan {code_label(expected)}, vraceno {code_label(actual)}")
            groups: dict[str, list[str]] = {}
            for name, message in items:
                groups.setdefault(POSITION_PATTERN.sub("L:S", message), []).append(name)
            for message, names in sorted(groups.items(), key=lambda item: -len(item[1]))[:5]:
                print(f"      {len(names)}x {message}")
                print(f"          napr. {names[0]}")
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
