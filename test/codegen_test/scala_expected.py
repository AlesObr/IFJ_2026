#!/usr/bin/env python3
"""
Ocekavane vystupy testu generatoru kodu ze Scaly (zadani, kapitola 4.1).

Kazdy <jmeno>.scala se spusti prikazem `scala run ifj26.scala prog.scala` (se vstupem
<jmeno>.in, je-li) a standardni vystup se ulozi do <jmeno>.out. Navratovy kod Scaly se
porovna s ocekavanym kodem z <jmeno>.code (vychozi 0); ifj26.scala konci kody 40 a 58 stejne
jako interpret, deleni nulou ale vyhodi vyjimku (kod 1), proto ma takovy test kod v .code
zadany rucne a nesoulad se jen oznami.

Rezim --filter-random slouzi pro nahodne programy: test, ktery ve Scale skonci chybou nebo
vypise cele cislo mimo +-10^8 (pravdepodobne preteceni 32bitoveho Int, ktere se v IFJcode26
s 64bitovym int chova jinak), se smaze.

Pouziti: python3 scala_expected.py --dir programs [--force] [--filter-random]
"""

import argparse
import os
import re
import shutil
import subprocess
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
IFJ26_SCALA = HERE.parent / "ifj26.scala"
BIG_INT = re.compile(r"(?<![\w.])-?\d{9,}(?![\w.])")


def java_env() -> dict:
    """JAVA_HOME podle `java` v PATH (jinak scala vypise varovani na standardni vystup)."""
    env = dict(os.environ)
    java = shutil.which("java")
    if java and "JAVA_HOME" not in env:
        env["JAVA_HOME"] = str(Path(os.path.realpath(java)).parent.parent)
    return env


def run_scala(program: Path, stdin: Path | None, env: dict) -> tuple[int, str, str]:
    with tempfile.TemporaryDirectory() as tmp:
        shutil.copy(IFJ26_SCALA, Path(tmp) / "ifj26.scala")
        shutil.copy(program, Path(tmp) / "prog.scala")
        with open(stdin if stdin else os.devnull, encoding="utf-8") as f:
            proc = subprocess.run(["scala", "run", "ifj26.scala", "prog.scala"], cwd=tmp, stdin=f,
                                  capture_output=True, text=True, env=env, timeout=300)
    # Pojistka: varovani o JAVA_HOME muze scala vypsat i na standardni vystup
    stdout = "".join(line for line in proc.stdout.splitlines(keepends=True)
                     if not line.startswith("Warning: JAVA_HOME"))
    return proc.returncode, stdout, proc.stderr


def main():
    parser = argparse.ArgumentParser(description="Ocekavane vystupy testu generatoru kodu ze Scaly")
    parser.add_argument("--dir", nargs="+", required=True, help="Adresare s programy *.scala")
    parser.add_argument("--force", action="store_true", help="Prepsat i existujici .out")
    parser.add_argument("--filter-random", action="store_true",
                        help="Smazat programy, ktere ve Scale selzou nebo vypisi velka cisla")
    parser.add_argument("--parser-bin", default=None,
                        help="S --filter-random smazat i programy, ktere neprojdou nasim parserem")
    args = parser.parse_args()

    env = java_env()
    done = removed = mismatched = 0
    for directory in args.dir:
        for program in sorted(Path(directory).glob("*.scala")):
            out_file = program.with_suffix(".out")
            if out_file.exists() and not args.force:
                continue
            if args.filter_random and args.parser_bin:
                with open(program, encoding="utf-8") as f:
                    if subprocess.run([args.parser_bin], stdin=f, capture_output=True).returncode != 0:
                        print(f"  smazan {program.name}: neprojde parserem")
                        program.unlink()
                        removed += 1
                        continue
            stdin = program.with_suffix(".in")
            rc, stdout, stderr = run_scala(program, stdin if stdin.exists() else None, env)
            code_file = program.with_suffix(".code")
            expected = int(code_file.read_text().strip()) if code_file.exists() else 0

            if args.filter_random and (rc != 0 or BIG_INT.search(stdout)):
                reason = f"Scala skoncila kodem {rc}" if rc != 0 else "velke cislo (preteceni Int?)"
                print(f"  smazan {program.name}: {reason}")
                for ext in (".scala", ".in", ".out", ".code"):
                    program.with_suffix(ext).unlink(missing_ok=True)
                removed += 1
                continue
            if rc != expected:
                last = (stderr.strip().splitlines() or [""])[-1][:150]
                print(f"  POZOR {program.name}: Scala skoncila kodem {rc}, test ceka {expected} ({last})")
                mismatched += 1
            out_file.write_text(stdout, encoding="utf-8")
            done += 1
    print(f"Hotovo: {done} vystupu, {removed} smazanych programu, {mismatched} s jinym kodem nez ocekavanym.")


if __name__ == "__main__":
    main()
