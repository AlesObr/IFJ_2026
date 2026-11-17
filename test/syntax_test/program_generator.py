#!/usr/bin/env python3
"""
Generator nahodnych syntakticky validnich programu v zakladnim jazyce IFJ26.

Programy slouzi jako dalsi seedy pro fuzzer i jako samostatne validni testy parseru.
Generuje typove konzistentni kod (promenne jen v dosahu, operandy stejneho typu),
aby se programy hodily i pro pozdejsi testy semantiky a generatoru kodu, a k tomu
schvalne nepekny, ale povoleny layout: odsazeni libovolnou sirkou, pokracovani
vyrazu za operatorem uvnitr zavorek, zalomeni uvnitr zavorek na libovolny hlubsi sloupec,
komentare mezi tokeny, stredniky, prazdne radky s mezerami, redundantni zavorky.

Rezim --stress generuje mezni programy (hluboke vnoreni, dlouhe vyrazy, mnoho
parametru a funkci, dlouhe identifikatory).

Validitu (dle parseru Scaly) a referencni .ast overuje ScalaAstOracle, viz fuzzer.py --oracle.
"""

import argparse
import random
from pathlib import Path

TYPES = ["Int", "Double", "String"]
# Scala/IFJ26 precedence: multiplicative > additive > relational (< > <= >=) > equality
PRECEDENCE = {"*": 4, "/": 4, "+": 3, "-": 3, "<": 2, ">": 2, "<=": 2, ">=": 2, "==": 1, "!=": 1}
RELATIONAL = ["<", ">", "<=", ">=", "==", "!="]
ESCAPES = ["\\n", "\\t", "\\\\", "\\\"", "\\r"]


class Scope:
    """Promenne viditelne v bloku (jmeno -> (typ, je_var))."""

    def __init__(self, parent: "Scope | None" = None):
        self.parent = parent
        self.names: dict[str, tuple[str, bool]] = {}

    def all(self) -> dict[str, tuple[str, bool]]:
        result = self.parent.all() if self.parent else {}
        result.update(self.names)
        return result

    def declared_here(self, name: str) -> bool:
        return name in self.names


class Expr:
    """Vyraz jako strom; text se sklada az pri vypisu (kvuli zavorkam a zalamovani)."""

    def __init__(self, kind: str, value: str = "", children: list["Expr"] | None = None):
        self.kind = kind          # lit, id, bin, un, call, unit
        self.value = value        # literal, jmeno, operator nebo jmeno funkce
        self.children = children or []

    def precedence(self) -> int:
        if self.kind == "bin":
            return PRECEDENCE[self.value]
        if self.kind == "un":
            return 5
        return 6


class ProgramGenerator:
    def __init__(self, rng: random.Random, stress: bool = False):
        self.rng = rng
        self.stress = stress
        self.functions: list[tuple[str, list[str], str]] = []  # (jmeno, typy parametru, navratovy typ)
        self.counter = 0
        self.max_expr_depth = 4

    # --- Pomocne ---

    def chance(self, p: float) -> bool:
        return self.rng.random() < p

    def fresh_name(self, prefix: str) -> str:
        self.counter += 1
        style = self.rng.random()
        if style < 0.1:
            return f"{prefix}_{self.counter}"
        if style < 0.2:
            return f"{prefix}{self.counter}X"
        if style < 0.25:
            return f"_{prefix}{self.counter}"
        return f"{prefix}{self.counter}"

    def comment(self) -> str:
        texts = ["/* c */", "/**/", "/* a /* nested */ b */", "/* * / */"]
        return self.rng.choice(texts)

    def ws(self) -> str:
        """Mezera mezi tokeny, obcas s komentarem nebo vice mezerami."""
        r = self.rng.random()
        if r < 0.06:
            return f" {self.comment()} "
        if r < 0.12:
            return " " * self.rng.randint(2, 4)
        return " "

    # --- Literaly ---

    def int_literal(self) -> str:
        value = self.rng.choice([0, 1, 2, 3, 7, 10, 42, 100, self.rng.randint(0, 99999)])
        if value >= 1000 and self.chance(0.3):
            text = f"{value:,}".replace(",", "_")
        else:
            text = str(value)
        return text

    def double_literal(self) -> str:
        forms = [
            lambda: f"{self.rng.randint(0, 999)}.{self.rng.randint(0, 99)}",
            lambda: f".{self.rng.randint(1, 99)}",
            lambda: f"{self.rng.randint(1, 9)}e{self.rng.randint(0, 5)}",
            lambda: f"{self.rng.randint(1, 9)}E+{self.rng.randint(0, 3)}",
            lambda: f"{self.rng.randint(1, 99)}.{self.rng.randint(0, 9)}e-{self.rng.randint(1, 3)}",
            lambda: f"{self.rng.randint(0, 9)}.{self.rng.randint(0, 9)}{self.rng.choice('dD')}",
            lambda: f"1_000.2_5",
            lambda: f"{self.rng.randint(1, 9)}e2d",
        ]
        return self.rng.choice(forms)()

    def string_literal(self) -> str:
        parts = []
        for _ in range(self.rng.randint(0, 4)):
            r = self.rng.random()
            if r < 0.25:
                parts.append(self.rng.choice(ESCAPES))
            elif r < 0.35:
                # Znaky, ktere vypadaji jako syntaxe, uvnitr retezce nesmi nic rozbit
                parts.append(self.rng.choice(["//", "/*", "*/", "(", ")", "=", "then", ";", ","]))
            else:
                parts.append(self.rng.choice(["a", "text", " ", "Hello", "x y", "0"]))
        return '"' + "".join(parts) + '"'

    def literal(self, typ: str) -> Expr:
        if typ == "Int":
            return Expr("lit", self.int_literal())
        if typ == "Double":
            return Expr("lit", self.double_literal())
        return Expr("lit", self.string_literal())

    # --- Vyrazy ---

    def expr(self, typ: str, scope: Scope, depth: int = 0) -> Expr:
        variables = [n for n, (t, _) in scope.all().items() if t == typ]
        callable_fns = [f for f in self.functions if f[2] == typ]
        leaf = depth >= self.max_expr_depth or self.chance(0.3 + 0.15 * depth)

        if leaf:
            if variables and self.chance(0.55):
                return Expr("id", self.rng.choice(variables))
            return self.literal(typ)

        r = self.rng.random()
        if r < 0.12 and callable_fns:
            name, params, _ = self.rng.choice(callable_fns)
            return Expr("call", name, [self.expr(p, scope, depth + 1) if p != "Unit" else Expr("unit")
                                       for p in params])
        if r < 0.2 and typ in ("Int", "Double"):
            return Expr("un", "-", [self.expr(typ, scope, depth + 1)])
        if r < 0.25 and typ == "String":
            return Expr("call", "toStr", [self.expr(self.rng.choice(["Int", "Double"]), scope, depth + 1)])

        ops = ["+"] if typ == "String" else ["+", "-", "*", "/"]
        op = self.rng.choice(ops)
        return Expr("bin", op, [self.expr(typ, scope, depth + 1), self.expr(typ, scope, depth + 1)])

    def condition(self, scope: Scope) -> Expr:
        op = self.rng.choice(RELATIONAL)
        typ = self.rng.choice(["Int", "Double"] if op not in ("==", "!=") else TYPES)
        return Expr("bin", op, [self.expr(typ, scope, 1), self.expr(typ, scope, 1)])

    def render(self, e: Expr, indent: int, parent_prec: int = 0, right: bool = False,
               in_parens: bool = False) -> str:
        """Vypise vyraz; zavorky tam, kde je vyzaduje priorita, obcas i navic."""
        text = self._render_inner(e, indent, in_parens)
        prec = e.precedence()
        needs = prec < parent_prec or (right and prec == parent_prec and e.kind == "bin")
        # Relacni operatory nejsou v IFJ26 asociativni, porovnani uvnitr porovnani ma vzdy zavorky
        if e.kind == "bin" and parent_prec in (1, 2) and prec in (1, 2):
            needs = True
        if needs or (e.kind in ("bin", "un", "lit", "id") and self.chance(0.06)):
            inner = self._render_inner(e, indent, True) if not in_parens else text
            if self.chance(0.1):
                # Zalomeni uvnitr zavorek na libovolny sloupec (LF v zavorkach je bily znak)
                # Zaviraci zavorka hloubeji nez blok: na nizsim sloupci by ji Scala brala jako
                # vynoreni z bloku (IFJ26 ne), oracle by pak dal jiny strom
                # Dle zadani 1.2 musi byt kazdy dalsi radek v zavorkach odsazen vic nez zacatek
                # logickeho radku (indent je odsazeni prikazu, ve kterem vyraz lezi)
                return "(\n" + " " * (indent + self.rng.randint(1, 12)) + inner + "\n" + " " * (indent + self.rng.randint(1, 8)) + ")"
            return "(" + inner + ")"
        return text

    def _render_inner(self, e: Expr, indent: int, in_parens: bool) -> str:
        if e.kind in ("lit", "id"):
            return e.value
        if e.kind == "unit":
            return "()"
        if e.kind == "un":
            child = self.render(e.children[0], indent, 5, in_parens=in_parens)
            # Scala povoli jen jeden prefixovy operator: "- -x" ne, "-(-x)" ano
            if child.startswith("-"):
                child = f"({child})"
            sep = " " if self.chance(0.2) else ""
            return f"-{sep}{child}"
        if e.kind == "call":
            args = [self.render(c, indent, 0, in_parens=True) for c in e.children]
            if args and self.chance(0.15):
                pieces = [",\n" + " " * (indent + self.rng.randint(1, 10)) + a for a in args[1:]]
                return f"{e.value}(\n{' ' * (indent + self.rng.randint(1, 10))}{args[0]}{''.join(pieces)}\n{' ' * (indent + self.rng.randint(1, 6))})"
            sep = "," + self.ws()
            return f"{e.value}({sep.join(args)})"
        left = self.render(e.children[0], indent, e.precedence(), in_parens=in_parens)
        right = self.render(e.children[1], indent, e.precedence(), right=True, in_parens=in_parens)
        if in_parens and self.chance(0.08):
            # Operator na konci radku: vyraz pokracuje na dalsim radku. Jen v zavorkach, jinak
            # jde dle zadani 1.2 o chybu (neukonceny vyraz nemuze pokracovat na dalsim logickem radku)
            if right.startswith("-"):
                right = f"({right})"
            return f"{left} {e.value}\n{' ' * (indent + self.rng.randint(1, 6))}{right}"
        if self.chance(0.1) and e.value in "+-*/" and not right.startswith("-"):
            return f"{left}{e.value}{right}"
        return f"{left}{self.ws()}{e.value}{self.ws()}{right}"

    # --- Prikazy ---

    def block(self, indent: int, scope: Scope, ret_type: str | None, depth: int, size: int | None = None) -> list[str]:
        """Vrati radky bloku; prvni radek je vzdy prikaz (blok nesmi byt prazdny)."""
        lines: list[str] = []
        count = size if size is not None else self.rng.randint(1, 5 if depth < 3 else 2)
        for i in range(count):
            lines.extend(self.statement(indent, scope, ret_type, depth))
            if self.chance(0.06):
                lines.append(" " * self.rng.randint(0, 10))  # prazdny radek jen s mezerami
            if self.chance(0.05):
                lines.append(" " * self.rng.randint(0, indent) + "// " + self.rng.choice(["komentar", "x = (", "*/"]))
        return lines

    def statement(self, indent: int, scope: Scope, ret_type: str | None, depth: int) -> list[str]:
        pad = " " * indent
        r = self.rng.random()
        mutable = [n for n, (_, is_var) in scope.all().items() if is_var]
        compound_ok = depth < (8 if self.stress else 4)

        if r < 0.3:
            typ = self.rng.choice(TYPES)
            name = self.fresh_name("v")
            keyword = self.rng.choice(["val", "var"])
            annotation = f":{self.ws()}{typ}" if self.chance(0.4) else ""
            value = self.render(self.expr(typ, scope), indent)
            scope.names[name] = (typ, keyword == "var")
            return [f"{pad}{keyword}{self.ws()}{name}{annotation}{self.ws()}={self.ws()}{value}"]
        if r < 0.42 and mutable:
            name = self.rng.choice(mutable)
            typ = scope.all()[name][0]
            return [f"{pad}{name}{self.ws()}={self.ws()}{self.render(self.expr(typ, scope), indent)}"]
        if r < 0.55:
            typ = self.rng.choice(TYPES)
            return [f"{pad}print({self.render(self.expr(typ, scope), indent, in_parens=True)})"]
        if r < 0.68 and compound_ok:
            return self.if_statement(indent, scope, ret_type, depth)
        if r < 0.78 and compound_ok:
            return self.while_statement(indent, scope, ret_type, depth)
        if r < 0.83 and ret_type is not None:
            if ret_type == "Unit":
                return [f"{pad}return"]
            return [f"{pad}return {self.render(self.expr(ret_type, scope), indent)}"]
        if r < 0.88:
            first = f"print({self.render(self.literal(self.rng.choice(TYPES)), indent)})"
            second = f"print({self.render(self.literal('Int'), indent)})"
            return [f"{pad}{first};{self.ws()}{second}"]
        if r < 0.92:
            return [f"{pad}()"]
        unit_fns = [f for f in self.functions if f[2] == "Unit"]
        if unit_fns:
            name, params, _ = self.rng.choice(unit_fns)
            call = Expr("call", name, [self.expr(p, scope, 1) if p != "Unit" else Expr("unit") for p in params])
            return [f"{pad}{self.render(call, indent)}"]
        return [f"{pad}print(\"\\n\")"]

    def child_indent(self, indent: int) -> int:
        return indent + self.rng.choice([1, 2, 2, 2, 3, 4, 4, 8])

    def if_statement(self, indent: int, scope: Scope, ret_type: str | None, depth: int) -> list[str]:
        pad = " " * indent
        cond = self.render(self.condition(scope), indent)
        lines = [f"{pad}if{self.ws()}{cond}{self.ws()}then"]
        inner = self.child_indent(indent)
        lines += self.block(inner, Scope(scope), ret_type, depth + 1)
        # Zakladni jazyk vyzaduje vetev else (bez ni jde o rozsireni BOOLTHEN)
        lines.append(f"{pad}else")
        lines += self.block(self.child_indent(indent), Scope(scope), ret_type, depth + 1)
        return lines

    def while_statement(self, indent: int, scope: Scope, ret_type: str | None, depth: int) -> list[str]:
        pad = " " * indent
        counter = self.fresh_name("i")
        limit = self.rng.randint(0, 5)
        lines = [f"{pad}var {counter} = 0",
                 f"{pad}while{self.ws()}{counter} < {limit}{self.ws()}do"]
        inner = self.child_indent(indent)
        body_scope = Scope(scope)
        lines += self.block(inner, body_scope, ret_type, depth + 1)
        lines.append(f"{' ' * inner}{counter} = {counter} + 1")
        scope.names[counter] = ("Int", False)  # citac se v tele dal neprepisuje
        return lines

    # --- Funkce a program ---

    def function(self, is_main: bool = False) -> list[str]:
        name = "main" + str(self.counter) if is_main else self.fresh_name("f")
        param_count = 0 if is_main else self.rng.randint(0, 40 if self.stress and self.chance(0.3) else 4)
        params = [(self.fresh_name("p"), self.rng.choice(TYPES + ["Unit"])) for _ in range(param_count)]
        ret_type = "Unit" if is_main else self.rng.choice(TYPES + ["Unit"])
        scope = Scope()
        for p, t in params:
            if t != "Unit":
                scope.names[p] = (t, False)

        param_text = ("," + self.ws()).join(f"{p}{self.ws().rstrip() or ''}:{self.ws()}{t}" for p, t in params)
        if params and self.chance(0.15):
            # Parametry pres vice radku s libovolnym odsazenim a koncovou carkou
            param_text = "\n" + ",\n".join(" " * self.rng.randint(1, 8) + f"{p}: {t}" for p, t in params) + \
                ",\n" + " " * self.rng.randint(1, 4)
        header = f"def{self.ws()}{name}({param_text}):{self.ws()}{ret_type}{self.ws()}="
        if is_main:
            header = ("@main\n" if self.chance(0.2) else "@main ") + header

        # Jednoradkove telo jen u funkce s vyrazem (bez return); jinak blok
        if not is_main and ret_type != "Unit" and self.chance(0.2):
            body = self.render(self.expr(ret_type, scope), 0)
            self.functions.append((name, [t for _, t in params], ret_type))
            return [f"{header} {body}"]

        indent = self.rng.choice([1, 2, 2, 2, 3, 4, 8])
        size = self.rng.randint(25, 60) if self.stress and self.chance(0.3) else None
        lines = [header] + self.block(indent, scope, ret_type, 0, size)
        if ret_type not in ("Unit",) and not is_main:
            # Posledni vyraz bloku je navratova hodnota
            last = self.render(self.expr(ret_type, scope), indent)
            # Radek zacinajici "- " by Scala vzala jako pokracovani predchoziho radku (leading infix)
            if last.startswith("-"):
                last = f"({last})"
            lines.append(" " * indent + last)
        self.functions.append((name, [t for _, t in params], ret_type))
        return lines

    def program(self) -> str:
        head = []
        if self.chance(0.2):
            head.append(self.rng.choice(["// prolog nasleduje", "/* komentar\n pred prologem */"]))
        head.append(self.rng.choice(["import ifj26.*", "import ifj26.*", "import   ifj26  .  *", "import ifj26 . *"]))
        body = []
        fn_count = self.rng.randint(1, 60 if self.stress and self.chance(0.2) else 5)
        for _ in range(fn_count):
            body.append("")
            if self.chance(0.1):
                body.append(self.rng.choice(["// funkce", "/* blok\n   pres radky */"]))
            body += self.function()
        body.append("")
        body += self.function(is_main=True)
        return "\n".join(head + body) + "\n"


def stress_programs() -> dict[str, str]:
    """Deterministicke mezni programy (hloubka, delka, pocet)."""
    programs = {}
    deep = 200
    programs["stress_deep_parens"] = (
        "import ifj26.*\n\n@main def main(): Unit =\n  val x = " + "(" * deep + "1" + ")" * deep + "\n  print(x)\n"
    )
    programs["stress_long_chain"] = (
        "import ifj26.*\n\n@main def main(): Unit =\n  val x = " + " + ".join(str(i % 10) for i in range(2000)) + "\n  print(x)\n"
    )
    programs["stress_mixed_chain"] = (
        "import ifj26.*\n\n@main def main(): Unit =\n  val a = 3\n  val x = "
        + " ".join(f"a {op}" for op in ["*", "+", "-", "/"] * 300) + " 1\n  print(x)\n"
    )
    programs["stress_unary_chain"] = (
        # "- - 1" je dle zadani platne (unarni minus je pravotocivy), ale parser Scaly ho
        # nezpracuje spravne (oracle by vypsal jiny strom), proto zavorky
        "import ifj26.*\n\n@main def main(): Unit =\n  val x = " + "-(" * 300 + "1" + ")" * 300 + "\n  print(x)\n"
    )
    lines = ["import ifj26.*", "", "@main def main(): Unit =", "  var x = 0"]
    for level in range(60):
        pad = "  " * (level + 1)
        lines += [f"{pad}if x < {level + 1} then", f"{pad}  x = x + 1"]
    for level in reversed(range(60)):
        pad = "  " * (level + 1)
        lines += [f"{pad}else", f"{pad}  ()"]
    lines.append("  print(x)")
    programs["stress_deep_if"] = "\n".join(lines) + "\n"

    lines = ["import ifj26.*", "", "@main def main(): Unit ="]
    for level in range(40):
        pad = " " * (level + 2)
        lines += [f"{pad}var w{level} = 0", f"{pad}while w{level} < 1 do", f"{pad} w{level} = w{level} + 1"]
    lines.append(" " * 42 + "print(1)")
    programs["stress_deep_while"] = "\n".join(lines) + "\n"

    params = ", ".join(f"p{i}: Int" for i in range(200))
    args = ", ".join(str(i) for i in range(200))
    programs["stress_many_params"] = (
        f"import ifj26.*\n\ndef wide({params}): Int = p0 + p199\n\n@main def main(): Unit =\n  print(wide({args}))\n"
    )
    funcs = "\n".join(f"def f{i}(x: Int): Int = x + {i}" for i in range(500))
    programs["stress_many_functions"] = f"import ifj26.*\n\n{funcs}\n\n@main def main(): Unit =\n  print(f499(1))\n"
    name = "v" + "x" * 1000
    programs["stress_long_identifier"] = (
        f"import ifj26.*\n\n@main def main(): Unit =\n  val {name} = 1\n  print({name})\n"
    )
    programs["stress_long_string"] = (
        "import ifj26.*\n\n@main def main(): Unit =\n  print(\"" + "abc\\n" * 5000 + "\")\n"
    )
    programs["stress_nested_calls"] = (
        "import ifj26.*\n\ndef id(s: String): String = s\n\n@main def main(): Unit =\n  print("
        + "id(" * 150 + '"x"' + ")" * 150 + ")\n"
    )
    stmts = "\n".join(f"  val s{i} = {i}" for i in range(5000))
    programs["stress_many_statements"] = f"import ifj26.*\n\n@main def main(): Unit =\n{stmts}\n  print(s4999)\n"
    programs["stress_blank_and_comments"] = (
        "import ifj26.*\n" + "\n   \n// c\n/* b */\n" * 300
        + "@main def main(): Unit =\n" + "\n      \n  // x\n" * 300 + "  print(1)\n" + "\n \n" * 100
    )
    return programs


def main():
    parser = argparse.ArgumentParser(description="Generator nahodnych validnich programu IFJ26")
    parser.add_argument("--out-dir", type=str, default="seed/random", help="Vystupni adresar")
    parser.add_argument("--count", type=int, default=50, help="Pocet nahodnych programu")
    parser.add_argument("--stress", action="store_true", help="Pridat i mezni (stress) programy")
    parser.add_argument("--seed", type=int, default=None, help="Seed nahodneho generatoru")
    args = parser.parse_args()

    rng = random.Random(args.seed)
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    for i in range(1, args.count + 1):
        generator = ProgramGenerator(rng, stress=args.stress and rng.random() < 0.2)
        (out_dir / f"random_{i:03d}.scala").write_text(generator.program(), encoding="utf-8")
    if args.stress:
        for name, code in stress_programs().items():
            (out_dir / f"{name}.scala").write_text(code, encoding="utf-8")
    print(f"Hotovo: {args.count} programu{' + stress' if args.stress else ''} v '{out_dir}/'.")


if __name__ == "__main__":
    main()
