#!/usr/bin/env python3
"""Red Team Fuzz Test Runner & Triage Engine pro IFJ26.

Zpracovává mutované vstupy, provádí diferenciální verifikaci proti referenci,
kategorizuje chyby, extrahuje fingerprinty havárií a minimalizuje zdrojové kódy.
"""

import argparse
from dataclasses import dataclass
import hashlib
from pathlib import Path
import re
import subprocess
import sys
import tempfile
from typing import Optional


@dataclass
class FuzzMetadata:
  seed: str = "unknown"
  mutation_type: str = "UNKNOWN"
  target_line: int = -1
  expected_exit: int = 0


@dataclass
class ExecutionResult:
  exit_code: int
  stdout: str
  stderr: str
  fingerprint: str
  is_crash: bool


class FuzzTriageRunner:

  def __init__(
      self,
      compiler_bin: Path,
      failures_dir: Path,
      ref_bin: Optional[Path] = None,
      int_bin: Optional[Path] = None,
      valgrind: bool = False,
      timeout: int = 3,
  ):
    self.compiler_bin = compiler_bin.resolve()
    self.failures_dir = failures_dir.resolve()
    self.ref_bin = ref_bin.resolve() if ref_bin else None
    self.int_bin = int_bin.resolve() if int_bin else None
    self.valgrind = valgrind
    self.timeout = timeout
    self.failures_dir.mkdir(parents=True, exist_ok=True)

  def parse_metadata(self, content: str) -> FuzzMetadata:
    """Extrahuje hlavičku metadat vloženou fuzzerem."""
    meta = FuzzMetadata()
    match = re.search(
        r"// === FUZZ_METADATA ===(.*?)// =====================",
        content,
        re.DOTALL,
    )
    if not match:
      return meta

    for line in match.group(1).strip().splitlines():
      line = line.strip().lstrip("//").strip()
      if ":" not in line:
        continue
      key, val = [x.strip() for x in line.split(":", 1)]
      if key == "SEED":
        meta.seed = val
      elif key == "MUTATION_TYPE":
        meta.mutation_type = val
      elif key == "TARGET_LINE":
        meta.target_line = int(val) if val.isdigit() else -1
      elif key == "EXPECTED_EXIT":
        meta.expected_exit = int(val) if val.isdigit() else 0
    return meta

  def run_reference(self, source_code: str) -> tuple[int, str]:
    """Spustí referenční interpret (pokud je zadán)."""
    if not self.ref_bin:
      return 0, ""
    try:
      proc = subprocess.run(
          [str(self.ref_bin)],
          input=source_code,
          text=True,
          capture_output=True,
          timeout=self.timeout,
          errors="replace",
      )
      return proc.returncode, proc.stdout.strip()
    except subprocess.TimeoutExpired:
      return -1, "REF_TIMEOUT"

  def execute_code(self, source_code: str) -> ExecutionResult:
    """Spustí testovaný kód pod ASanem/Valgrindem a volitelně prožene mezikód přes ic26int."""
    compile_cmd = []
    if self.valgrind:
      compile_cmd = [
          "valgrind",
          "--leak-check=full",
          "--show-leak-kinds=all",
          "--error-exitcode=42",
          str(self.compiler_bin),
      ]
    else:
      compile_cmd = [str(self.compiler_bin)]

    try:
      c_proc = subprocess.run(
          compile_cmd,
          input=source_code,
          text=True,
          capture_output=True,
          timeout=self.timeout,
          errors="replace",
      )
    except subprocess.TimeoutExpired:
      return ExecutionResult(
          124, "", "TIMEOUT v překladači", "TIMEOUT", is_crash=True
      )

    # Detekce havárií
    is_asan = (
        "AddressSanitizer" in c_proc.stderr
        or "UndefinedBehaviorSanitizer" in c_proc.stderr
    )
    is_segv = c_proc.returncode in [139, -11, 134, -6]
    is_leak = (c_proc.returncode == 42) or (
        "definitely lost:" in c_proc.stderr
        and "0 bytes in 0 blocks" not in c_proc.stderr
    )
    is_crash = is_segv or is_asan or is_leak

    if is_crash:
      if is_asan:
        asan_match = re.search(
            r"(AddressSanitizer:\s*\w+.*?)\s+at\s+([^\n]+)",
            c_proc.stderr,
            re.DOTALL,
        )
        raw = asan_match.group(0) if asan_match else "ASAN_GENERIC"
        fp = "asan_" + hashlib.md5(raw.encode()).hexdigest()[:8]
      elif is_leak:
        fp = (
            "valgrind_leak_"
            + hashlib.md5(c_proc.stderr.encode()).hexdigest()[:8]
        )
      else:
        fp = f"segv_signal_{abs(c_proc.returncode)}"
      return ExecutionResult(
          c_proc.returncode, "", c_proc.stderr, fp, is_crash=True
      )

    # Pokud překladač ohlásil chybu (1..99), nepokračujeme do interpretu
    if c_proc.returncode != 0:
      return ExecutionResult(
          c_proc.returncode,
          "",
          c_proc.stderr,
          f"exit_{c_proc.returncode}",
          is_crash=False,
      )

    # Pokud máme fakultní interpret ic26int, spustíme vygenerovaný mezikód
    if self.int_bin:
      try:
        i_proc = subprocess.run(
            [str(self.int_bin)],
            input=c_proc.stdout,
            text=True,
            capture_output=True,
            timeout=self.timeout,
            errors="replace",
        )
        return ExecutionResult(
            i_proc.returncode,
            i_proc.stdout.strip(),
            i_proc.stderr,
            "CLEAN",
            is_crash=False,
        )
      except subprocess.TimeoutExpired:
        return ExecutionResult(
            124, "", "TIMEOUT v ic26int", "TIMEOUT", is_crash=True
        )

    return ExecutionResult(
        c_proc.returncode,
        c_proc.stdout.strip(),
        c_proc.stderr,
        "CLEAN",
        is_crash=False,
    )

  def shrink_source(self, source_code: str, target_fingerprint: str) -> str:
    """Delta minimalizátor: Řádek po řádku odřezává kód, dokud identický crash přetrvává."""
    lines = source_code.splitlines(keepends=True)
    i = 0
    while i < len(lines):
      if "FUZZ_METADATA" in lines[i] or lines[i].startswith("//"):
        i += 1
        continue

      candidate = "".join(lines[:i] + lines[i + 1 :])
      res = self.execute_code(candidate)

      if res.fingerprint == target_fingerprint:
        lines = lines[:i] + lines[i + 1 :]
      else:
        i += 1

    return "".join(lines)

  def triage_file(self, file_path: Path) -> str:
    content = file_path.read_text(encoding="utf-8", errors="replace")
    meta = self.parse_metadata(content)
    our = self.execute_code(content)

    # 1. CRITICAL: Havárie nebo memory leak překladače
    if our.is_crash:
      crash_dir = self.failures_dir / f"CRASH_{our.fingerprint}"
      if not crash_dir.exists():
        crash_dir.mkdir(parents=True, exist_ok=True)
        shrunk_code = self.shrink_source(content, our.fingerprint)
        (crash_dir / "minimized.ifj").write_text(shrunk_code, encoding="utf-8")
        (crash_dir / "original.ifj").write_text(content, encoding="utf-8")
        (crash_dir / "crash_report.txt").write_text(
            f"MUTATION: {meta.mutation_type}\n"
            f"ORIGINAL SEED: {meta.seed}\n"
            f"EXIT CODE: {our.exit_code}\n\n"
            f"STDERR:\n{our.stderr}\n",
            encoding="utf-8",
        )
      return f"\033[31m[CRASH: {our.fingerprint}]\033[0m {file_path.name}"

    # 2. DIFERENCIÁLNÍ REŽIM (pokud je dostupná reference)
    if self.ref_bin:
      ref_exit, ref_stdout = self.run_reference(content)
      if ref_exit != 0:
        if our.exit_code == 0:
          err_dir = self.failures_dir / "FALSE_POSITIVE_ACCEPT"
          err_dir.mkdir(parents=True, exist_ok=True)
          (err_dir / file_path.name).write_text(content, encoding="utf-8")
          return (
              f"\033[33m[ERROR MISMATCH]\033[0m {file_path.name}: Reference"
              f" kód odmítla ({ref_exit}), ale my přeložili!"
          )
        return (
            f"\033[32m[PASS: EXPECTED REJECT]\033[0m {file_path.name} (Oba"
            " správně odmítli)"
        )

      if our.exit_code != 0:
        err_dir = self.failures_dir / f"FALSE_REJECT_code_{our.exit_code}"
        err_dir.mkdir(parents=True, exist_ok=True)
        (err_dir / file_path.name).write_text(content, encoding="utf-8")
        return (
            f"\033[33m[FALSE REJECT]\033[0m {file_path.name}: Reference"
            f" prošla, ale my hlásíme {our.exit_code}!"
        )

      if self.int_bin and our.stdout != ref_stdout:
        diff_dir = self.failures_dir / "OUTPUT_MISMATCH"
        diff_dir.mkdir(parents=True, exist_ok=True)
        (diff_dir / file_path.name).write_text(
            f"--- REF STDOUT ---\n{ref_stdout}\n--- OUR STDOUT"
            f" ---\n{our.stdout}\n\n--- SOURCE ---\n{content}",
            encoding="utf-8",
        )
        return (
            f"\033[31m[OUTPUT DIFF]\033[0m {file_path.name}: Výstup nesedí s"
            " referencí!"
        )
      return f"\033[32m[PASS: FULL MATCH]\033[0m {file_path.name}"

    # 3. METADATA REŽIM (podle hlavičky očekávaného exit kódu)
    if our.exit_code != meta.expected_exit:
      mismatch_dir = (
          self.failures_dir
          / f"MISMATCH_code_{our.exit_code}_exp_{meta.expected_exit}"
      )
      mismatch_dir.mkdir(parents=True, exist_ok=True)
      (mismatch_dir / file_path.name).write_text(content, encoding="utf-8")
      return (
          f"\033[33m[CONTRACT VIOLATION]\033[0m {file_path.name} (Got:"
          f" {our.exit_code}, Expected: {meta.expected_exit})"
      )

    return f"\033[32m[PASS]\033[0m {file_path.name}"


def run_self_test():
  print("=== SPOUŠTÍM SELF-TEST PRO FUZZ RUNNER ===", flush=True)
  with tempfile.TemporaryDirectory() as tmpdir:
    tmp_path = Path(tmpdir)
    mock_src = tmp_path / "mock_compiler.c"
    mock_bin = tmp_path / "mock_compiler"
    failures = tmp_path / "failures"

    print(f"[1/4] Vytvářím mock překladač v {tmp_path}...", flush=True)
    mock_src.write_text(
        r"""
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char buf[1024] = {0};
    if (fread(buf, 1, sizeof(buf) - 1, stdin) == 0) {
        return 0;
    }
    if (strstr(buf, "CRASH")) {
        volatile int *p = NULL;
        *p = 42; // Vynutí SIGSEGV
    }
    if (strstr(buf, "LEX_ERR")) {
        return 1;
    }
    return 0;
}
""",
        encoding="utf-8",
    )

    print("[2/4] Kompiluji mock překladač...", flush=True)
    build_proc = subprocess.run(
        ["gcc", "-O0", "-g", str(mock_src), "-o", str(mock_bin)],
        capture_output=True,
        text=True,
    )
    if build_proc.returncode != 0:
      print(
          f"\033[31mChyba kompilace mock překladače:\033[0m\n{build_proc.stderr}",
          flush=True,
      )
      sys.exit(1)

    runner = FuzzTriageRunner(
        compiler_bin=mock_bin, failures_dir=failures, timeout=2
    )

    fuzz_sample = """// === FUZZ_METADATA ===
// SEED: sample.src
// MUTATION_TYPE: INJECT_CRASH
// EXPECTED_EXIT: 0
// =====================
var a = 10;
var b = 20;
CRASH
var c = 30;
"""
    test_file = tmp_path / "test_crash.ifj"
    test_file.write_text(fuzz_sample, encoding="utf-8")

    print("[3/4] Spouštím triage a delta-minimalizátor...", flush=True)
    status = runner.triage_file(test_file)
    print(f"Výsledek triage: {status}", flush=True)

    print("[4/4] Ověřuji zredukovaný kód...", flush=True)
    crash_dirs = list(failures.glob("CRASH_*"))
    if not crash_dirs:
      print(
          "\033[31mChyba: Složka s crashem nebyla nalezena!\033[0m", flush=True
      )
      sys.exit(1)

    minimized = (crash_dirs[0] / "minimized.ifj").read_text(encoding="utf-8")
    print("--- Původní kód ---")
    print(fuzz_sample.strip())
    print("--- Zredukovaný kód ---")
    print(minimized.strip())
    print("-----------------------", flush=True)

    assert "CRASH" in minimized, "Shrinker smazal kritický řádek!"
    assert "var a" not in minimized, "Shrinker neosekal nepotřebné řádky!"

    print(
        "\n\033[32m✓ SELF-TEST ÚSPĚŠNÝ: Runner detekuje crashe, hashuje podpisy"
        " a minimalizuje kód.\033[0m\n",
        flush=True,
    )


def main():
  parser = argparse.ArgumentParser(
      description="IFJ26 Red Team Fuzz Runner & Shrinker"
  )
  parser.add_argument("--bin", type=Path, help="Cesta k našemu překladači")
  parser.add_argument(
      "--in-dir", type=Path, help="Složka s mutovanými .ifj soubory"
  )
  parser.add_argument(
      "--ref-bin",
      type=Path,
      help="Cesta k referenčnímu interpretu (volitelné orakulum)",
  )
  parser.add_argument(
      "--int-bin",
      type=Path,
      help="Cesta k fakultnímu interpretu mezikódu ic26int",
  )
  parser.add_argument(
      "--failures-dir",
      type=Path,
      default=Path("failures"),
      help="Kam ukládat deduplikované errory",
  )
  parser.add_argument(
      "--valgrind", action="store_true", help="Spustit pod Valgrindem"
  )
  parser.add_argument(
      "--self-test",
      action="store_true",
      help="Ověřit funkčnost samotného runneru",
  )
  args = parser.parse_args()

  if args.self_test:
    run_self_test()
    sys.exit(0)

  if not args.bin or not args.in_dir:
    parser.print_help()
    sys.exit(1)

  runner = FuzzTriageRunner(
      compiler_bin=args.bin,
      failures_dir=args.failures_dir,
      ref_bin=args.ref_bin,
      int_bin=args.int_bin,
      valgrind=args.valgrind,
  )

  files = sorted(list(args.in_dir.glob("*.ifj")))
  print(
      f">>> Zahajuji analýzu {len(files)} vygenerovaných případů <<<",
      flush=True,
  )
  for f in files:
    print(runner.triage_file(f), flush=True)


if __name__ == "__main__":
  main()