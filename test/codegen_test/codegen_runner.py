#!/usr/bin/env python3
"""
Test runner generatoru kodu IFJ26 (end-to-end: prekladac -> interpret -> porovnani vystupu).

Test je <jmeno>.scala s ocekavanym vystupem <jmeno>.out (ze Scaly, viz scala_expected.py),
volitelnym vstupem <jmeno>.in a ocekavanym navratovym kodem interpretu <jmeno>.code (vychozi 0).
Test projde, pokud prekladac skonci kodem 0, interpret kodem z .code a standardni vystup se
shoduje s .out.

Interpret: --interp, jinak ic26int z PATH (promenna IC26INT), jinak nahradni ifjcode_interp.py
(neoficialni, chovani se muze v detailech lisit).

Konstrukce pouzite v testu (if, while, unarni minus, konkatenace ...) se rozpoznaji ze
zdrojoveho kodu. --exclude if,while preskoci testy, ktere nektere z nich pouzivaji (generator
kodu, ktery je jeste neumi, se tak da testovat po castech); souhrn ukazuje uspesnost podle
konstrukci.

Pouziti: python3 codegen_runner.py --bin ../../build/codegen_test --dir programs random
             [--exclude if,while] [--keep DIR]
"""

import argparse
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / "syntax_test"))
from fuzzer import IFJCodeMutator  # noqa: E402  (spolecne maskovani retezcu a komentaru)

# Konstrukce rozpoznavane ve zdrojovem kodu (v kodu s retezci nahrazenymi "S" a bez komentaru)
OPERAND_END = r'[\w)"]'
FEATURES = {
    "if": re.compile(r"\bif\b"),
    "while": re.compile(r"\bwhile\b"),
    "for": re.compile(r"\bfor\b"),
    "return": re.compile(r"\breturn\b"),
    # Minus, pred kterym neni operand (zacatek vyrazu, za operatorem, zavorkou ci carkou)
    "unary_minus": re.compile(r"(?:^|[=(,+\-*/<>]|\bthen|\belse|\breturn|\bdo)[ \t]*-(?=[ \t]*[\w(.\-])", re.M),
    "neq": re.compile(r"!="),
    "le": re.compile(r"<="),
    "ge": re.compile(r">="),
    "eq": re.compile(r"=="),
    "lt": re.compile(r"<(?!=)"),
    "gt": re.compile(r"(?<![=\-])>(?!=)"),
    "div": re.compile(r"(?<![/*])/(?![/*])"),
    "mul": re.compile(r"(?<!/)\*(?!/)"),
    # Priblizne: + vedle retezcoveho literalu nebo volani funkce vracejici String
    "concat": re.compile(r'"S"\s*\+|\+\s*"S"|\b(?:toStr|subStr|chr|readLine)\s*\([^()]*\)\s*\+'
                         r'|\+\s*(?:toStr|subStr|chr|readLine)\b'),
    # FUNEXP: volani jako argument nebo operand (ne jako cela prava strana ci prikaz)
    "funexp": re.compile(r"(?:[(,+\-*/<>]|[=!]=)\s*[A-Za-z_]\w*\s*\((?!\s*\))|"
                         r"[A-Za-z_]\w*\s*\([^()]*\)\s*[-+*/<>]"),
    "read_input": re.compile(r"\bread(?:Line|Int|Double)\s*\("),
}


def source_features(test: Path) -> set[str]:
    code = test.read_text(encoding="utf-8")
    masked = list(code)
    for start, end in IFJCodeMutator._literal_spans(code):
        replacement = '"S"' if code[start] == '"' else " "
        masked[start:end] = [""] * (end - start)
        masked[start] = replacement
    clean = "".join(masked)
    # Prolog "import ifj26.*" by se jinak pocital jako nasobeni
    clean = re.sub(r"(?m)^\s*import\b.*$", "", clean)
    found = {name for name, pattern in FEATURES.items() if pattern.search(clean)}
    code_file = test.with_suffix(".code")
    if code_file.exists() and code_file.read_text().strip() != "0":
        found.add("runtime_error")
    return found
FALLBACK_INTERP = HERE / "ifjcode_interp.py"
VALGRIND_EXIT_CODE = 77
COMPILE_TIMEOUT = 10
RUN_TIMEOUT = 30


def find_interpreter(requested: str | None) -> list[str]:
    if requested:
        return [requested] if not requested.endswith(".py") else [sys.executable, requested]
    for candidate in (os.environ.get("IC26INT"), shutil.which("ic26int")):
        if candidate:
            return [candidate]
    return [sys.executable, str(FALLBACK_INTERP)]


def last_line(text: str) -> str:
    lines = [line.strip() for line in text.splitlines() if line.strip()]
    return lines[-1][:160] if lines else "(prazdny vypis)"


def first_difference(expected: str, actual: str) -> str:
    exp_lines, act_lines = expected.splitlines(), actual.splitlines()
    for index in range(max(len(exp_lines), len(act_lines))):
        exp = exp_lines[index] if index < len(exp_lines) else "<konec>"
        act = act_lines[index] if index < len(act_lines) else "<konec>"
        if exp != act:
            return f"radek {index + 1}: ocekavano {exp[:70]!r}, vypsano {act[:70]!r}"
    return "lisi se jen koncove odradkovani"


def run_test(args, interp: list[str], test: Path) -> tuple[bool, str, str]:
    """Returns (passed, failure kind, detail)."""
    cmd = ["valgrind", "-q", f"--error-exitcode={VALGRIND_EXIT_CODE}", "--leak-check=no"] if args.valgrind else []
    try:
        with open(test, encoding="utf-8") as f:
            comp = subprocess.run(cmd + [args.bin], stdin=f, capture_output=True, text=True,
                                  timeout=COMPILE_TIMEOUT * (10 if args.valgrind else 1))
    except subprocess.TimeoutExpired:
        return False, "Prekladac: timeout", ""
    if comp.returncode < 0:
        return False, f"Prekladac: pad (signal {-comp.returncode})", ""
    if args.valgrind and comp.returncode == VALGRIND_EXIT_CODE:
        return False, "Prekladac: Valgrind (neplatny pristup do pameti)", last_line(comp.stderr)
    if comp.returncode != 0:
        return False, f"Prekladac: kod {comp.returncode}", last_line(comp.stderr)

    code_path = Path(args.keep) / f"{test.stem}.code" if args.keep else None
    if code_path:
        code_path.write_text(comp.stdout, encoding="utf-8")
    program = code_path or Path(args.tmp) / "prog.code"
    if not code_path:
        program.write_text(comp.stdout, encoding="utf-8")

    stdin_path = test.with_suffix(".in")
    try:
        with open(stdin_path if stdin_path.exists() else os.devnull, encoding="utf-8") as f:
            run = subprocess.run(interp + [str(program)], stdin=f, capture_output=True, text=True,
                                 timeout=RUN_TIMEOUT)
    except subprocess.TimeoutExpired:
        return False, "Interpret: timeout (zacykleni?)", ""
    if args.keep:
        (Path(args.keep) / f"{test.stem}.actual").write_text(run.stdout, encoding="utf-8")

    code_file = test.with_suffix(".code")
    expected_code = int(code_file.read_text().strip()) if code_file.exists() else 0
    if run.returncode != expected_code:
        return False, f"Interpret: kod {run.returncode} (ocekavan {expected_code})", last_line(run.stderr)
    expected = test.with_suffix(".out").read_text(encoding="utf-8")
    if run.stdout != expected:
        return False, "Vystup se lisi", first_difference(expected, run.stdout)
    return True, "", ""


def main():
    parser = argparse.ArgumentParser(description="Codegen runner pro IFJ26")
    parser.add_argument("--bin", default="../../build/codegen_test", help="Prekladac (parser + semantika + codegen)")
    parser.add_argument("--dir", nargs="+", default=["programs"], help="Adresare s testy (*.scala + *.out)")
    parser.add_argument("--interp", default=None, help="Interpret IFJcode26 (vychozi ic26int, jinak ifjcode_interp.py)")
    parser.add_argument("--keep", default=None, help="Ulozit vygenerovany kod a skutecny vystup do adresare")
    parser.add_argument("--valgrind", action="store_true", help="Prekladac spoustet pod Valgrindem")
    parser.add_argument("--filter", default=None, help="Jen testy, jejichz nazev obsahuje tento text")
    parser.add_argument("--exclude", default="",
                        help=f"Preskocit testy s temito konstrukcemi (carkou oddelene): {', '.join(FEATURES)}, runtime_error")
    parser.add_argument("--features", action="store_true", help="Jen vypsat konstrukce kazdeho testu")
    parser.add_argument("-v", "--verbose", action="store_true", help="Vypsat vysledek kazdeho testu")
    args = parser.parse_args()

    if not Path(args.bin).is_file():
        print(f"Prekladac '{args.bin}' neexistuje. Nejprve sestavte cil codegen-build.")
        sys.exit(1)
    args.bin = str(Path(args.bin).resolve())
    candidates = sorted(t for d in args.dir for t in Path(d).glob("*.scala")
                        if t.with_suffix(".out").exists() and (args.filter is None or args.filter in t.name))
    excluded = {name.strip() for name in args.exclude.split(",") if name.strip()}
    unknown = excluded - set(FEATURES) - {"runtime_error"}
    if unknown:
        print(f"Neznama konstrukce v --exclude: {', '.join(sorted(unknown))}")
        sys.exit(1)
    features = {t: source_features(t) for t in candidates}
    if args.features:
        for test in candidates:
            print(f"{test.parent.name}/{test.name}: {' '.join(sorted(features[test])) or '-'}")
        return
    tests = [t for t in candidates if not features[t] & excluded]
    skipped = len(candidates) - len(tests)
    if not tests:
        print(f"Nenalezeny zadne testy (*.scala s *.out) v {args.dir}"
              f"{f' ({skipped} preskoceno kvuli --exclude)' if skipped else ''}.")
        sys.exit(1)
    if args.keep:
        Path(args.keep).mkdir(parents=True, exist_ok=True)

    interp = find_interpreter(args.interp)
    official = not interp[-1].endswith("ifjcode_interp.py")
    skip_info = f", {skipped} preskoceno (--exclude {','.join(sorted(excluded))})" if skipped else ""
    print(f"Spoustim {len(tests)} testu{skip_info}, interpret: {' '.join(interp)}"
          f"{'' if official else ' (NEOFICIALNI nahrada, ic26int nenalezen)'}\n")

    import tempfile
    failures: dict[str, list[tuple[str, str]]] = {}
    per_dir: dict[str, list[int]] = {}
    per_feature: dict[str, list[int]] = {}
    with tempfile.TemporaryDirectory() as tmp:
        args.tmp = tmp
        for test in tests:
            ok, kind, detail = run_test(args, interp, test)
            per_dir.setdefault(test.parent.name, [0, 0])[0 if ok else 1] += 1
            for feature in features[test] or {"(zadna)"}:
                per_feature.setdefault(feature, [0, 0])[0 if ok else 1] += 1
            if not ok:
                failures.setdefault(kind, []).append((f"{test.parent.name}/{test.name}", detail))
            if args.verbose:
                print(f"[{'PASS' if ok else 'FAIL'}] {test.parent.name}/{test.name}"
                      + ("" if ok else f" -> {kind}: {detail}"))

    print("=" * 60)
    total_ok = sum(v[0] for v in per_dir.values())
    total_fail = sum(v[1] for v in per_dir.values())
    for name, (ok, fail) in sorted(per_dir.items()):
        total = ok + fail
        print(f"{name}: {ok} proslo, {fail} selhalo z {total} testu ({100 * ok / total:.1f} %).")
    print(f"Celkem: {total_ok} proslo, {total_fail} selhalo z {total_ok + total_fail} testu "
          f"({100 * total_ok / (total_ok + total_fail):.1f} %).")
    if skipped:
        print(f"Preskoceno: {skipped} testu s konstrukcemi {', '.join(sorted(excluded))}.")

    print("\nUspesnost podle konstrukci (test muze pouzivat vic konstrukci):")
    for feature, (ok, fail) in sorted(per_feature.items(), key=lambda item: item[1][0] / sum(item[1])):
        print(f"  {feature:13s} {ok:4d}/{ok + fail:<4d} ({100 * ok / (ok + fail):5.1f} %)")

    if failures:
        print(f"\nAnalyza selhani ({total_fail} testu):")
        for kind, items in sorted(failures.items(), key=lambda item: -len(item[1])):
            print(f"  {len(items)}x {kind}")
            groups: dict[str, list[str]] = {}
            for name, detail in items:
                groups.setdefault(detail, []).append(name)
            for detail, names in sorted(groups.items(), key=lambda item: -len(item[1]))[:4]:
                print(f"      {len(names)}x {detail}" if detail else f"      {len(names)}x")
                print(f"          napr. {names[0]}")
    sys.exit(1 if total_fail else 0)


if __name__ == "__main__":
    main()
