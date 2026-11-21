#!/usr/bin/env python3
"""
Stav rozsireni po fazich: pro kazdy program z extensions/ (<rozsireni>_<konstrukce>.scala) ukaze,
kam az dojde: parser -> semantika -> generator kodu -> beh v interpretu -> shoda vystupu.

Pouziti: python3 ext_status.py [--dir extensions] [--build ../../build]
"""

import argparse
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from codegen_runner import find_interpreter  # noqa: E402

STAGES = ["parser", "semantika", "generator", "beh", "vystup", "OK"]


def last_message(stderr: str) -> str:
    lines = [line.strip() for line in stderr.splitlines() if line.strip() and line.strip() != '"']
    for line in reversed(lines):
        match = re.search(r'message="([^"]*)', line)
        if match:
            return match.group(1).strip()
        if line.startswith("["):
            return re.sub(r"^\[\w+\]\s*", "", line)
    return lines[-1] if lines else ""


def run(cmd: list[str], stdin_path: Path | None) -> subprocess.CompletedProcess:
    with open(stdin_path if stdin_path and stdin_path.exists() else os.devnull, encoding="utf-8") as f:
        return subprocess.run(cmd, stdin=f, capture_output=True, text=True, timeout=30)


def status(test: Path, build: Path, interp: list[str], tmp: Path) -> tuple[str, str]:
    """Returns (stage where the program stopped, detail)."""
    parsed = run([str(build / "parser_test")], test)
    if parsed.returncode != 0:
        return "parser", f"kod {parsed.returncode}: {last_message(parsed.stderr)}"
    semantic = run([str(build / "semantic_test")], test)
    if semantic.returncode != 0:
        return "semantika", f"kod {semantic.returncode}: {last_message(semantic.stderr)}"
    compiled = run([str(build / "ifj26")], test)
    if compiled.returncode != 0:
        return "generator", f"kod {compiled.returncode}: {last_message(compiled.stderr)}"
    program = tmp / "prog.code"
    program.write_text(compiled.stdout, encoding="utf-8")
    result = run(interp + [str(program)], test.with_suffix(".in"))
    code_file = test.with_suffix(".code")
    expected_code = int(code_file.read_text().strip()) if code_file.exists() else 0
    if result.returncode != expected_code:
        return "beh", f"interpret kod {result.returncode} (ocekavan {expected_code}): {last_message(result.stderr)}"
    expected = test.with_suffix(".out").read_text(encoding="utf-8")
    if result.stdout != expected:
        return "vystup", f"ocekavano {expected[:40]!r}, vypsano {result.stdout[:40]!r}"
    return "OK", ""


def main():
    parser = argparse.ArgumentParser(description="Stav rozsireni po fazich prekladu")
    parser.add_argument("--dir", default=str(HERE / "extensions"))
    parser.add_argument("--build", default=str(HERE.parent.parent / "build"), help="Adresar s parser_test, semantic_test a ifj26")
    parser.add_argument("--interp", default=None)
    args = parser.parse_args()

    build = Path(args.build)
    for binary in ("parser_test", "semantic_test", "ifj26"):
        if not (build / binary).is_file():
            print(f"Chybi {build / binary}: sestavte make parser-build semantic-build compiler")
            sys.exit(1)
    interp = find_interpreter(args.interp)
    tests = sorted(t for t in Path(args.dir).glob("*.scala") if t.with_suffix(".out").exists())

    groups: dict[str, list[tuple[str, str, str]]] = {}
    with tempfile.TemporaryDirectory() as tmp:
        for test in tests:
            stage, detail = status(test, build, interp, Path(tmp))
            ext, _, feature = test.stem.partition("_")
            groups.setdefault(ext.upper(), []).append((feature, stage, detail))

    print(f"Interpret: {' '.join(interp)}\n")
    print(f"{'Rozsireni':12s} {'OK':>5s}   zaseknuti ve fazi")
    for ext, items in groups.items():
        counts = {stage: sum(1 for _, s, _ in items if s == stage) for stage in STAGES}
        stuck = ", ".join(f"{stage} {n}" for stage, n in counts.items() if n and stage != "OK")
        print(f"{ext:12s} {counts['OK']:>2d}/{len(items):<2d}   {stuck or '-'}")
    for ext, items in groups.items():
        print(f"\n{ext}")
        for feature, stage, detail in items:
            mark = "OK " if stage == "OK" else "-- "
            print(f"  {mark}{feature:28s} {'' if stage == 'OK' else stage + ': ' + detail[:95]}")


if __name__ == "__main__":
    main()
