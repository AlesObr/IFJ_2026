#!/usr/bin/env python3
"""
Zjednoduseny interpret IFJcode26 dle kapitoly 7 zadani (nahrada za ic26int pro lokalni testy).

Neni to oficialni interpret: slouzi k rychlemu overeni generatoru kodu, dokud neni k dispozici
ic26int. Navratove kody chyb odpovidaji zadani (52-58); presne hranicni chovani (rozsahy cisel,
format nekterych hodnot) se muze od ic26int lisit.

Pouziti: python3 ifjcode_interp.py program.code < vstup > vystup
"""

import re
import sys


class InterpError(Exception):
    def __init__(self, code: int, message: str):
        super().__init__(message)
        self.code = code


class Nil:
    def __repr__(self):
        return "nil"


NIL = Nil()
UNINIT = object()


def c_hex(value: float) -> str:
    """Float in the C %a format (Python's float.hex keeps trailing zeros)."""
    if value != value:
        return "nan"
    if value in (float("inf"), float("-inf")):
        return "inf" if value > 0 else "-inf"
    text = value.hex()
    sign = ""
    if text.startswith("-"):
        sign, text = "-", text[1:]
    mantissa, exponent = text[2:].split("p")
    if "." in mantissa:
        whole, frac = mantissa.split(".")
        frac = frac.rstrip("0")
        mantissa = whole + ("." + frac if frac else "")
    if mantissa == "0":
        exponent = "+0"
    elif not exponent.startswith(("-", "+")):
        exponent = "+" + exponent
    return f"{sign}0x{mantissa}p{exponent}"


def type_name(value) -> str:
    if value is UNINIT:
        return ""
    if value is NIL:
        return "nil"
    if isinstance(value, bool):
        return "bool"
    if isinstance(value, int):
        return "int"
    if isinstance(value, float):
        return "float"
    if isinstance(value, str):
        return "string"
    return "array"


def unescape(text: str) -> str:
    return re.sub(r"\\(\d{3})", lambda m: chr(int(m.group(1))), text)


def parse_const(token: str):
    kind, _, value = token.partition("@")
    if kind == "int":
        return int(value, 0) if value.lower().startswith(("0x", "-0x", "0o", "-0o")) else int(value)
    if kind == "float":
        return float.fromhex(value) if "x" in value.lower() else float(value)
    if kind == "bool":
        return value == "true"
    if kind == "string":
        return unescape(value)
    if kind == "nil":
        return NIL
    raise InterpError(51, f"neznamy literal {token}")


def wrap64(value: int) -> int:
    value &= (1 << 64) - 1
    return value - (1 << 64) if value >= 1 << 63 else value


class Machine:
    def __init__(self, code: str, stdin):
        self.stdin = stdin
        self.out = []
        self.instructions = []
        self.labels = {}
        lines = code.splitlines()
        if not lines or lines[0].split("#")[0].strip() != ".IFJcode26":
            raise InterpError(51, "chybi hlavicka .IFJcode26")
        for line in lines[1:]:
            line = line.split("#", 1)[0].strip()
            if not line:
                continue
            parts = line.split()
            opcode = parts[0].upper()
            if opcode == "LABEL":
                if parts[1] in self.labels:
                    raise InterpError(52, f"redefinice navesti {parts[1]}")
                self.labels[parts[1]] = len(self.instructions)
            self.instructions.append((opcode, parts[1:]))
        self.gf = {}
        self.frames = []
        self.tf = None
        self.stack = []
        self.calls = []

    # --- operands ---

    def frame(self, name: str) -> dict:
        prefix = name.split("@", 1)[0]
        if prefix == "GF":
            return self.gf
        if prefix == "LF":
            if not self.frames:
                raise InterpError(55, "LF neexistuje")
            return self.frames[-1]
        if prefix == "TF":
            if self.tf is None:
                raise InterpError(55, "TF neexistuje")
            return self.tf
        raise InterpError(51, f"spatna promenna {name}")

    def is_var(self, token: str) -> bool:
        return token.split("@", 1)[0] in ("GF", "LF", "TF")

    def read(self, token: str):
        if not self.is_var(token):
            return parse_const(token)
        frame = self.frame(token)
        key = token.split("@", 1)[1]
        if key not in frame:
            raise InterpError(54, f"neexistujici promenna {token}")
        if frame[key] is UNINIT:
            raise InterpError(56, f"neinicializovana promenna {token}")
        return frame[key]

    def read_maybe_uninit(self, token: str):
        if not self.is_var(token):
            return parse_const(token)
        frame = self.frame(token)
        key = token.split("@", 1)[1]
        if key not in frame:
            raise InterpError(54, f"neexistujici promenna {token}")
        return frame[key]

    def write(self, token: str, value):
        frame = self.frame(token)
        key = token.split("@", 1)[1]
        if key not in frame:
            raise InterpError(54, f"neexistujici promenna {token}")
        frame[key] = value

    def pop(self):
        if not self.stack:
            raise InterpError(56, "prazdny datovy zasobnik")
        return self.stack.pop()

    def jump(self, label: str) -> int:
        if label not in self.labels:
            raise InterpError(52, f"neexistujici navesti {label}")
        return self.labels[label]

    # --- operations ---

    @staticmethod
    def arith(op: str, a, b):
        if type_name(a) != type_name(b) or type_name(a) not in ("int", "float"):
            raise InterpError(53, f"{op}: spatne typy {type_name(a)} {type_name(b)}")
        if op == "DIV":
            if type_name(a) != "float":
                raise InterpError(53, "DIV vyzaduje float")
            if b == 0:
                raise InterpError(57, "deleni nulou")
            return a / b
        if op == "IDIV":
            if type_name(a) != "int":
                raise InterpError(53, "IDIV vyzaduje int")
            if b == 0:
                raise InterpError(57, "deleni nulou")
            q = abs(a) // abs(b)
            return q if (a >= 0) == (b >= 0) else -q
        result = {"ADD": lambda: a + b, "SUB": lambda: a - b, "MUL": lambda: a * b}[op]()
        return wrap64(result) if isinstance(result, int) else result

    @staticmethod
    def relation(op: str, a, b) -> bool:
        ta, tb = type_name(a), type_name(b)
        if op == "EQ":
            if ta == "nil" or tb == "nil":
                return ta == tb
            if ta != tb:
                raise InterpError(53, f"EQ: spatne typy {ta} {tb}")
            return a == b
        if ta != tb or ta == "nil" or ta not in ("int", "float", "string", "bool"):
            raise InterpError(53, f"{op}: spatne typy {ta} {tb}")
        return a < b if op == "LT" else a > b

    def read_line(self):
        line = self.stdin.readline()
        return None if line == "" else line

    def do_read(self, kind: str):
        line = self.read_line()
        if line is None:
            return NIL
        if kind == "string":
            return line
        text = line[:-1] if line.endswith("\n") else line
        if kind == "int":
            return int(text) if re.fullmatch(r"[+-]?\d+", text) else NIL
        if kind == "float":
            try:
                if re.fullmatch(r"[+-]?0[xX][0-9a-fA-F]*\.?[0-9a-fA-F]*([pP][+-]?\d+)?", text):
                    return float.fromhex(text)
                if re.fullmatch(r"[+-]?(\d+\.?\d*|\.\d+)([eE][+-]?\d+)?", text):
                    return float(text)
            except ValueError:
                pass
            return NIL
        if kind == "bool":
            return True if text == "true" else (False if text == "false" else NIL)
        raise InterpError(53, f"READ: typ {kind}")

    @staticmethod
    def show(value) -> str:
        if value is NIL:
            return "nil"
        if isinstance(value, bool):
            return "true" if value else "false"
        if isinstance(value, float):
            return c_hex(value)
        return str(value)

    def run(self) -> int:
        pc = 0
        steps = 0
        unary_s = {"INT2FLOATS", "FLOAT2INTS", "INT2CHARS", "FLOAT2STRS", "INT2STRS", "NOTS", "TYPES"}
        while pc < len(self.instructions):
            steps += 1
            if steps > 50_000_000:
                raise InterpError(60, "prilis mnoho instrukci")
            opcode, args = self.instructions[pc]
            pc += 1
            if opcode == "LABEL":
                continue
            if opcode == "MOVE":
                self.write(args[0], self.read(args[1]))
            elif opcode == "CREATEFRAME":
                self.tf = {}
            elif opcode == "PUSHFRAME":
                if self.tf is None:
                    raise InterpError(55, "PUSHFRAME bez TF")
                self.frames.append(self.tf)
                self.tf = None
            elif opcode == "POPFRAME":
                if not self.frames:
                    raise InterpError(55, "POPFRAME bez LF")
                self.tf = self.frames.pop()
            elif opcode == "DEFVAR":
                frame = self.frame(args[0])
                key = args[0].split("@", 1)[1]
                if key in frame:
                    raise InterpError(52, f"redefinice promenne {args[0]}")
                frame[key] = UNINIT
            elif opcode == "CALL":
                target = self.jump(args[0])
                self.calls.append(pc)
                pc = target
            elif opcode == "RETURN":
                if not self.calls:
                    raise InterpError(56, "RETURN bez CALL")
                pc = self.calls.pop()
            elif opcode == "PUSHS":
                self.stack.append(self.read(args[0]))
            elif opcode == "POPS":
                self.write(args[0], self.pop())
            elif opcode == "CLEARS":
                self.stack.clear()
            elif opcode in ("ADD", "SUB", "MUL", "DIV", "IDIV"):
                self.write(args[0], self.arith(opcode, self.read(args[1]), self.read(args[2])))
            elif opcode in ("ADDS", "SUBS", "MULS", "DIVS", "IDIVS"):
                b, a = self.pop(), self.pop()
                self.stack.append(self.arith(opcode[:-1], a, b))
            elif opcode in ("LT", "GT", "EQ"):
                self.write(args[0], self.relation(opcode, self.read(args[1]), self.read(args[2])))
            elif opcode in ("LTS", "GTS", "EQS"):
                b, a = self.pop(), self.pop()
                self.stack.append(self.relation(opcode[:-1], a, b))
            elif opcode in ("AND", "OR"):
                a, b = self.read(args[1]), self.read(args[2])
                if not isinstance(a, bool) or not isinstance(b, bool):
                    raise InterpError(53, f"{opcode}: spatne typy")
                self.write(args[0], (a and b) if opcode == "AND" else (a or b))
            elif opcode in ("ANDS", "ORS"):
                b, a = self.pop(), self.pop()
                if not isinstance(a, bool) or not isinstance(b, bool):
                    raise InterpError(53, f"{opcode}: spatne typy")
                self.stack.append((a and b) if opcode == "ANDS" else (a or b))
            elif opcode == "NOT" or opcode in unary_s:
                stack = opcode in unary_s
                value = self.pop() if stack else self.read(args[1])
                base = opcode[:-1] if stack else opcode
                result = self.convert(base, value)
                if stack:
                    self.stack.append(result)
                else:
                    self.write(args[0], result)
            elif opcode in ("INT2FLOAT", "FLOAT2INT", "INT2CHAR", "FLOAT2STR", "INT2STR", "TYPE"):
                value = self.read_maybe_uninit(args[1]) if opcode == "TYPE" else self.read(args[1])
                self.write(args[0], self.convert(opcode, value))
            elif opcode in ("STRI2INT", "STRI2INTS", "GETCHAR"):
                if opcode == "STRI2INTS":
                    index, text = self.pop(), self.pop()
                else:
                    text, index = self.read(args[1]), self.read(args[2])
                if not isinstance(text, str) or type_name(index) != "int":
                    raise InterpError(53, f"{opcode}: spatne typy")
                if not 0 <= index < len(text):
                    raise InterpError(58, f"{opcode}: index mimo retezec")
                result = text[index] if opcode == "GETCHAR" else ord(text[index])
                if opcode == "STRI2INTS":
                    self.stack.append(result)
                else:
                    self.write(args[0], result)
            elif opcode == "READ":
                self.write(args[0], self.do_read(args[1]))
            elif opcode == "WRITE":
                self.out.append(self.show(self.read(args[0])))
            elif opcode == "CONCAT":
                a, b = self.read(args[1]), self.read(args[2])
                if not isinstance(a, str) or not isinstance(b, str):
                    raise InterpError(53, "CONCAT: spatne typy")
                self.write(args[0], a + b)
            elif opcode == "STRLEN":
                text = self.read(args[1])
                if not isinstance(text, str):
                    raise InterpError(53, "STRLEN: spatny typ")
                self.write(args[0], len(text))
            elif opcode == "SETCHAR":
                text, index, char = self.read(args[0]), self.read(args[1]), self.read(args[2])
                if not 0 <= index < len(text) or char == "":
                    raise InterpError(58, "SETCHAR mimo retezec")
                self.write(args[0], text[:index] + char[0] + text[index + 1:])
            elif opcode == "JUMP":
                pc = self.jump(args[0])
            elif opcode in ("JUMPIFEQ", "JUMPIFNEQ", "JUMPIFEQS", "JUMPIFNEQS"):
                target = self.jump(args[0])
                if opcode.endswith("S"):
                    b, a = self.pop(), self.pop()
                else:
                    a, b = self.read(args[1]), self.read(args[2])
                equal = self.relation("EQ", a, b)
                if equal == opcode.startswith("JUMPIFEQ"):
                    pc = target
            elif opcode == "EXIT":
                code = self.read(args[0])
                if type_name(code) != "int" or not 0 <= code <= 49:
                    raise InterpError(57, "EXIT: spatny kod")
                return code
            elif opcode in ("BREAK", "DPRINT"):
                if opcode == "DPRINT":
                    sys.stderr.write(self.show(self.read(args[0])))
            else:
                raise InterpError(51, f"neznama instrukce {opcode}")
        return 0

    @staticmethod
    def convert(op: str, value):
        tn = type_name(value)
        if op == "TYPE":
            return tn
        if op == "NOT":
            if tn != "bool":
                raise InterpError(53, "NOT: spatny typ")
            return not value
        if op == "INT2FLOAT":
            if tn != "int":
                raise InterpError(53, "INT2FLOAT: spatny typ")
            return float(value)
        if op == "FLOAT2INT":
            if tn != "float":
                raise InterpError(53, "FLOAT2INT: spatny typ")
            return int(value)
        if op == "INT2CHAR":
            if tn != "int":
                raise InterpError(53, "INT2CHAR: spatny typ")
            if not 0 <= value <= 127:
                raise InterpError(58, "INT2CHAR mimo rozsah")
            return chr(value)
        if op == "FLOAT2STR":
            if tn != "float":
                raise InterpError(53, "FLOAT2STR: spatny typ")
            return f"{value:.2f}"
        if op == "INT2STR":
            if tn != "int":
                raise InterpError(53, "INT2STR: spatny typ")
            return str(value)
        raise InterpError(51, op)


def main():
    if len(sys.argv) != 2:
        print("Pouziti: ifjcode_interp.py program.code < vstup", file=sys.stderr)
        sys.exit(50)
    with open(sys.argv[1], encoding="ascii", errors="replace") as f:
        code = f.read()
    try:
        machine = Machine(code, sys.stdin)
        rc = machine.run()
    except InterpError as error:
        sys.stdout.write("".join(machine.out) if "machine" in locals() else "")
        print(f"ifjcode_interp: chyba {error.code}: {error}", file=sys.stderr)
        sys.exit(error.code)
    sys.stdout.write("".join(machine.out))
    sys.exit(rc)


if __name__ == "__main__":
    main()
