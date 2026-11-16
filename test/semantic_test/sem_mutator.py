#!/usr/bin/env python3
"""
Generator semantickych testu z validnich programu (seedu).

Kazdy validni seed se zkopiruje jako ok_seed_<jmeno>.scala (ocekavany kod 0) a z nej vznikaji
mutace s jedinou semantickou chybou, jejiz kod je dany zadanim:

  e3_undef_var     pouziti nedefinovane promenne (prejmenovany vyskyt)
  e3_undef_func    volani nedefinovane funkce
  e3_missing_main  odstranena anotace @main
  e4_redefine      definice promenne zopakovana ve stejnem bloku
  e5_arg_count     volani uzivatelske funkce s argumentem navic
  e6_decl_type     nekompatibilni typ v deklaraci (val x: String = 1)
  e6_concat        konkatenace bez toStr ("a" + 1)
  e7_assign_val    prirazeni do val
  e7_assign_param  prirazeni do parametru

Seedy musi byt semanticky spravne (napr. seed/basic a nahodne programy z program_generator.py).
Seedy, ktere neprojdou parserem (--parser-bin), se preskoci, aby chyba parseru nezkreslila
vysledek.

Pouziti: python3 sem_mutator.py --seeds-dir ../syntax_test/seed/basic ../syntax_test/seed/random \\
             --out-dir generated --per-seed 3 --seed 2026 --parser-bin ../../build/parser_test
"""

import argparse
import random
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "syntax_test"))
from fuzzer import IFJCodeMutator  # noqa: E402  (spolecne maskovani retezcu a komentaru)

DECL = re.compile(r"(?m)^( *)(val|var)\s+([A-Za-z_]\w*)\s*(:\s*(Int|Double|String)\s*)?=")
DEF = re.compile(r"(?m)^(?:@main\s+)?def\s+([A-Za-z_]\w*)\s*\(([^()]*)\)")
IDENT = re.compile(r"(?<![\w.])([A-Za-z_]\w*)\b")
BUILTINS = {"print", "readLine", "readInt", "readDouble", "toStr", "toInt", "strLength",
            "subStr", "ord", "chr"}


class Source:
    """Program s maskovanym kodem (retezce a komentare nahrazene mezerami)."""

    def __init__(self, code: str):
        self.code = code
        self.clean = IFJCodeMutator._strip_comments_and_strings(code)
        self.lines = code.splitlines(keepends=True)
        self.clean_lines = self.clean.splitlines(keepends=True)

    def line_of(self, pos: int) -> int:
        return self.code.count("\n", 0, pos)

    def simple_line(self, index: int) -> bool:
        """Radek je cely jeden logicky radek: vyvazene zavorky a nekonci operatorem."""
        clean = self.clean_lines[index].rstrip()
        return clean.count("(") == clean.count(")") and not re.search(r"[-+*/<>=,(]$", clean)

    def functions(self) -> dict[str, int]:
        """Uzivatelske funkce s jednoradkovym seznamem parametru -> pocet parametru."""
        result = {}
        for match in DEF.finditer(self.clean):
            params = match.group(2).strip()
            result[match.group(1)] = 0 if not params else params.count(",") + 1
        return result

    def call_sites(self, name: str) -> list[int]:
        """Pozice '(' u volani funkce name (ne u jeji definice)."""
        sites = []
        for match in re.finditer(rf"(?<![\w.]){re.escape(name)}\s*\(", self.clean):
            before = self.clean[:match.start()].rstrip()
            if not before.endswith("def"):
                sites.append(match.end() - 1)
        return sites

    def matching_paren(self, open_pos: int) -> int:
        depth = 0
        for pos in range(open_pos, len(self.clean)):
            if self.clean[pos] == "(":
                depth += 1
            elif self.clean[pos] == ")":
                depth -= 1
                if depth == 0:
                    return pos
        return -1


def insert_line(code: str, after_line: int, text: str) -> str:
    lines = code.splitlines(keepends=True)
    if not lines[after_line].endswith("\n"):
        lines[after_line] += "\n"
    return "".join(lines[:after_line + 1] + [text + "\n"] + lines[after_line + 1:])


# --- mutace: kazda vraci novy kod, nebo None, kdyz se na seed neda pouzit ---

def undef_var(src: Source, rng: random.Random) -> str | None:
    decls = list(DECL.finditer(src.clean))
    names = {m.group(3) for m in decls}
    # The declared name itself must stay (a comment may lie between val/var and the name)
    declared_at = {m.start(3) for m in decls}
    uses = [m for m in IDENT.finditer(src.clean) if m.group(1) in names and m.start() not in declared_at]
    if not uses:
        return None
    use = rng.choice(uses)
    return src.code[:use.start()] + use.group(1) + "_undefined" + src.code[use.end():]


def undef_func(src: Source, rng: random.Random) -> str | None:
    sites = [(name, pos) for name in src.functions() for pos in src.call_sites(name)]
    if not sites:
        return None
    name, pos = rng.choice(sites)
    start = src.clean.rfind(name, 0, pos)
    return src.code[:start] + name + "_undefined" + src.code[start + len(name):]


def missing_main(src: Source, rng: random.Random) -> str | None:
    match = re.search(r"@main\b\s*", src.clean)
    if match is None:
        return None
    return src.code[:match.start()] + src.code[match.end():]


def decl_lines(src: Source, keyword: str | None = None) -> list[re.Match]:
    return [m for m in DECL.finditer(src.clean) if (keyword is None or m.group(2) == keyword)
            and src.simple_line(src.line_of(m.start()))]


def redefine(src: Source, rng: random.Random) -> str | None:
    decls = decl_lines(src)
    if not decls:
        return None
    decl = rng.choice(decls)
    index = src.line_of(decl.start())
    return insert_line(src.code, index, src.lines[index].rstrip("\r\n"))


def assign_val(src: Source, rng: random.Random) -> str | None:
    decls = decl_lines(src, "val")
    if not decls:
        return None
    decl = rng.choice(decls)
    indent, name = decl.group(1), decl.group(3)
    return insert_line(src.code, src.line_of(decl.start()), f"{indent}{name} = {name}")


def assign_param(src: Source, rng: random.Random) -> str | None:
    candidates = []
    for match in DEF.finditer(src.clean):
        params = [p.split(":")[0].strip() for p in match.group(2).split(",") if ":" in p]
        index = src.line_of(match.start())
        header = src.clean_lines[index].rstrip()
        if params and header.endswith("=") and index + 1 < len(src.lines):
            body = src.lines[index + 1]
            indent = len(body) - len(body.lstrip(" "))
            if indent > 0 and body.strip():
                candidates.append((index, rng.choice(params), indent))
    if not candidates:
        return None
    index, param, indent = rng.choice(candidates)
    return insert_line(src.code, index, f"{' ' * indent}{param} = {param}")


def arg_count(src: Source, rng: random.Random) -> str | None:
    sites = [(name, pos, count) for name, count in src.functions().items()
             for pos in src.call_sites(name)]
    if not sites:
        return None
    _, pos, count = rng.choice(sites)
    close = src.matching_paren(pos)
    if close < 0:
        return None
    extra = "1" if count == 0 and not src.clean[pos + 1:close].strip() else ", 1"
    return src.code[:close] + extra + src.code[close:]


def decl_type(src: Source, rng: random.Random) -> str | None:
    decls = [m for m in decl_lines(src) if m.group(5)]
    if not decls:
        return None
    decl = rng.choice(decls)
    wrong = {"Int": "String", "Double": "String", "String": "Int"}[decl.group(5)]
    start, end = decl.span(5)
    return src.code[:start] + wrong + src.code[end:]


def concat(src: Source, rng: random.Random) -> str | None:
    """toStr(x) primo vedle '+' -> x: retezec + cislo bez konverze."""
    sites = []
    for match in re.finditer(r"(?<![\w.])toStr\s*\(", src.clean):
        close = src.matching_paren(match.end() - 1)
        before = src.clean[:match.start()].rstrip()
        after = src.clean[close + 1:].lstrip() if close > 0 else ""
        # Argument typu String by po odstraneni toStr zustal validni, bere se jen cislo/promenna
        if close > 0 and (before.endswith("+") or after.startswith("+")) and \
                not src.code[match.end():close].lstrip().startswith('"'):
            sites.append((match.start(), match.end(), close))
    if not sites:
        return None
    start, inner, close = rng.choice(sites)
    return src.code[:start] + "(" + src.code[inner:close] + ")" + src.code[close + 1:]


MUTATIONS = [
    ("e3_undef_var", undef_var), ("e3_undef_func", undef_func), ("e3_missing_main", missing_main),
    ("e4_redefine", redefine), ("e5_arg_count", arg_count), ("e6_decl_type", decl_type),
    ("e6_concat", concat), ("e7_assign_val", assign_val), ("e7_assign_param", assign_param),
]


def parses(parser_bin: str | None, path: Path) -> bool:
    if parser_bin is None:
        return True
    with open(path, encoding="utf-8") as f:
        return subprocess.run([parser_bin], stdin=f, capture_output=True, timeout=10).returncode == 0


def main():
    parser = argparse.ArgumentParser(description="Generator semantickych testu z validnich seedu")
    parser.add_argument("--seeds-dir", nargs="+", required=True, help="Adresare s validnimi programy")
    parser.add_argument("--out-dir", default="generated", help="Vystupni adresar (jeho .scala se prepisou)")
    parser.add_argument("--per-seed", type=int, default=3, help="Pocet mutaci na seed")
    parser.add_argument("--seed", type=int, default=None, help="Seed nahodneho generatoru")
    parser.add_argument("--parser-bin", default=None, help="Binarka parseru; seedy, ktere neprojdou, se preskoci")
    args = parser.parse_args()

    rng = random.Random(args.seed)
    out = Path(args.out_dir)
    out.mkdir(parents=True, exist_ok=True)
    for old in out.glob("*.scala"):
        old.unlink()

    seeds = sorted(f for d in args.seeds_dir for f in Path(d).glob("*.scala") if f.name != "ifj26.scala")
    counts: dict[str, int] = {}
    skipped = 0
    for seed in seeds:
        if not parses(args.parser_bin, seed):
            skipped += 1
            continue
        code = seed.read_text(encoding="utf-8")
        (out / f"ok_seed_{seed.stem}.scala").write_text(code, encoding="utf-8")
        counts["ok_seed"] = counts.get("ok_seed", 0) + 1
        src = Source(code)
        produced = {code}
        for i in range(args.per_seed):
            order = list(MUTATIONS)
            rng.shuffle(order)
            for tag, fn in order:
                mutated = fn(src, rng)
                # Deterministicka mutace (napr. odstraneni @main) by jinak vznikla vicekrat
                if mutated is not None and mutated not in produced:
                    produced.add(mutated)
                    (out / f"{tag}_{seed.stem}_{i}.scala").write_text(mutated, encoding="utf-8")
                    counts[tag] = counts.get(tag, 0) + 1
                    break
    summary = ", ".join(f"{tag} {n}x" for tag, n in sorted(counts.items()))
    print(f"Hotovo ({skipped} seedu preskoceno kvuli parseru): {summary}")


if __name__ == "__main__":
    main()
