#!/usr/bin/env python3
"""
Rucni testy generatoru kodu: program IFJ26, volitelny vstup a ocekavany navratovy kod.

Zapise do programs/ soubory <jmeno>.scala, <jmeno>.in (je-li vstup) a <jmeno>.code (je-li
ocekavany kod jiny nez 0). Ocekavany vystup <jmeno>.out vytvori scala_expected.py spustenim
programu prekladacem Scala s oficialni ifj26.scala.

Pouziti: python3 make_programs.py && python3 scala_expected.py --dir programs
"""

from pathlib import Path

OUT = Path(__file__).resolve().parent / "programs"
PROLOG = "import ifj26.*\n\n"

# name -> (popis, program bez prologu, vstup nebo None, ocekavany kod interpretu nebo None = 0)
PROGRAMS = {
    "print_int": ("print celych cisel (WRITE %d)", """
@main def main(): Unit =
  print(42)
  print("\\n")
  print(-7)
  print("\\n")
  print(0)
  print("\\n")
""", None, None),
    "print_double_format": ("print desetinnych cisel ve formatu %a", """
@main def main(): Unit =
  print(1.5)
  print("\\n")
  print(0.1)
  print("\\n")
  print(-2.0)
  print("\\n")
  print(0.0)
  print("\\n")
  print(1e10)
  print("\\n")
  val d: Double = 3
  print(d)
  print("\\n")
""", None, None),
    "negative_zero": ("unarni minus nad 0.0 zachova znamenko", """
@main def main(): Unit =
  val z = 0.0
  val n = -z
  print(n)
  print("\\n")
""", None, None),
    "tostr_double_rounding": ("toStr pro Double ve formatu %.2f", """
@main def main(): Unit =
  print(toStr(2.345))
  print("\\n")
  print(toStr(0.125))
  print("\\n")
  print(toStr(-0.004))
  print("\\n")
  print(toStr(1e6))
  print("\\n")
  print(toStr(2.5))
  print("\\n")
""", None, None),
    "tostr_other_types": ("toStr pro Int, String a Unit", """
@main def main(): Unit =
  val a = toStr(-5)
  val b = toStr("x")
  val c = toStr(())
  print(a + b + c)
  print("\\n")
""", None, None),
    "print_unit": ("print(()) vypise ()", """
@main def main(): Unit =
  print(())
  val u = ()
  print(u)
  print("\\n")
""", None, None),
    "string_escapes": ("escapovani mezery, #, \\\\, tabulatoru a uvozovek v konstantach", """
@main def main(): Unit =
  print("a#b\\\\c d\\te")
  print("\\n")
  print("\\"q\\" \\r")
  print("\\n")
""", None, None),
    "int_division": ("celociselne deleni zaokrouhluje k nule", """
@main def main(): Unit =
  val a = 7 / 2
  val b = -7 / 2
  val c = 7 / -2
  val d = -7 / -2
  print(a)
  print(" ")
  print(b)
  print(" ")
  print(c)
  print(" ")
  print(d)
  print("\\n")
""", None, None),
    "double_division": ("deleni s alespon jednim Double operandem", """
@main def main(): Unit =
  val a = 7.0 / 2
  val b = 1 / 4.0
  print(a)
  print(" ")
  print(b)
  print("\\n")
""", None, None),
    "implicit_conversions": ("implicitni konverze Int -> Double (deklarace, argument, navrat, prirazeni)", """
def half(x: Double): Double = x / 2

def one(): Double = 1

@main def main(): Unit =
  val d: Double = 2
  val h = half(3)
  var v = 0.5
  v = 4
  print(d / 4)
  print(" ")
  print(h)
  print(" ")
  print(one())
  print(" ")
  print(v)
  print("\\n")
""", None, None),
    "arithmetic_precedence": ("priority a asociativita operatoru", """
@main def main(): Unit =
  val a = 2 + 3 * 4 - 6 / 2
  val b = (2 + 3) * 4
  val c = -(3 - 5) * 2
  val d = 10 - 4 - 3
  val e = 100 / 10 / 5
  print(a)
  print(" ")
  print(b)
  print(" ")
  print(c)
  print(" ")
  print(d)
  print(" ")
  print(e)
  print("\\n")
""", None, None),
    "comparisons": ("relacni operatory nad Int, Double, String a smisenymi cisly", """
def show(label: String): Unit =
  print(label)
  print(" ")

@main def main(): Unit =
  if 1 < 2 then
    show("lt")
  else
    show("-")
  if 2 <= 2 then
    show("le")
  else
    show("-")
  if 3 > 2.5 then
    show("gt")
  else
    show("-")
  if 2.0 >= 2 then
    show("ge")
  else
    show("-")
  if 1 == 1.0 then
    show("eq")
  else
    show("-")
  if "abc" != "abd" then
    show("ne")
  else
    show("-")
  if "abc" < "abd" then
    show("slt")
  else
    show("-")
  print("\\n")
""", None, None),
    "concat_and_length": ("konkatenace a strLength", """
@main def main(): Unit =
  val s = "ab" + "cd"
  val t = s + "" + s
  print(t)
  print(" ")
  print(strLength(t))
  print(" ")
  print(strLength(""))
  print("\\n")
""", None, None),
    "substr_ok": ("subStr v mezich", """
@main def main(): Unit =
  print(subStr("hello", 1, 3))
  print(" ")
  print(subStr("abc", 0, 3))
  print(" ")
  print(subStr("abc", 2, 2))
  print("|\\n")
""", None, None),
    "substr_start_out_of_range": ("subStr se start >= delka konci chybou 40", """
@main def main(): Unit =
  print("before\\n")
  val s = subStr("abc", 3, 3)
  print(s)
""", None, 40),
    "substr_start_after_end": ("subStr se start > end konci chybou 40", """
@main def main(): Unit =
  print("before\\n")
  val s = subStr("abcdef", 4, 2)
  print(s)
""", None, 40),
    "ord_cases": ("ord vraci -1 mimo rozsah a pro prazdny retezec", """
@main def main(): Unit =
  print(ord("abc", 0))
  print(" ")
  print(ord("abc", 2))
  print(" ")
  print(ord("abc", 3))
  print(" ")
  print(ord("", 0))
  print(" ")
  print(ord("a", -1))
  print("\\n")
""", None, None),
    "chr_ok": ("chr v intervalu 0 az 127", """
@main def main(): Unit =
  val a = chr(65)
  val b = chr(126)
  print(a + b)
  print("\\n")
""", None, None),
    "chr_out_of_range": ("chr mimo 0 az 127 konci chybou 58", """
@main def main(): Unit =
  print("before\\n")
  val c = chr(200)
  print(c)
""", None, 58),
    "toint": ("toInt orezava desetinnou cast", """
@main def main(): Unit =
  print(toInt(3.9))
  print(" ")
  print(toInt(-3.9))
  print(" ")
  print(toInt(5))
  print("\\n")
""", None, None),
    "read_builtins": ("readLine/readInt/readDouble vc. chybneho vstupu a EOF", """
@main def main(): Unit =
  val line = readLine()
  val i = readInt()
  val bad = readInt()
  val d = readDouble()
  val e = readDouble()
  val spaced = readInt()
  val eofLine = readLine()
  val eofInt = readInt()
  print(line)
  print(i)
  print(" ")
  print(bad)
  print(" ")
  print(d)
  print(" ")
  print(e)
  print(" ")
  print(spaced)
  print(" [")
  print(eofLine)
  print("] ")
  print(eofInt)
  print("\\n")
""", "hello world\n42\nabc\n3.5\n1e2\n 7\n", None),
    "int_div_by_zero": ("celociselne deleni nulou konci chybou interpretu 57", """
@main def main(): Unit =
  print("before\\n")
  val zero = 0
  val x = 1 / zero
  print(x)
""", None, 57),
    "while_sum": ("cyklus while", """
@main def main(): Unit =
  var i = 1
  var sum = 0
  while i <= 10 do
    sum = sum + i
    i = i + 1
  print(sum)
  print("\\n")
""", None, None),
    "nested_loops": ("vnorene cykly a definice v tele (kazda iterace)", """
@main def main(): Unit =
  var row = 1
  while row <= 3 do
    var col = 0
    while col < row do
      val mark = "*"
      print(mark)
      col = col + 1
    print("\\n")
    row = row + 1
""", None, None),
    "if_else_chain": ("vnorene if/else", """
def classify(n: Int): String =
  if n < 0 then
    return "neg"
  else
    if n == 0 then
      return "zero"
    else
      return "pos"

@main def main(): Unit =
  print(classify(-3))
  print(classify(0))
  print(classify(8))
  print("\\n")
""", None, None),
    "shadowing": ("prekryti promenne v podbloku, puvodni hodnota zustane", """
@main def main(): Unit =
  val x = 1
  if x < 2 then
    val x = "inner"
    print(x)
  else
    ()
  print(x)
  var y = 10
  while y > 8 do
    val y2 = y
    print(y2)
    y = y - 1
  print("\\n")
""", None, None),
    "factorial_recursive": ("rekurze", """
def factorial(n: Int): Int =
  if n < 2 then
    return 1
  else
    val m = n - 1
    val sub = factorial(m)
    return n * sub

@main def main(): Unit =
  print(factorial(10))
  print("\\n")
""", None, None),
    "mutual_recursion": ("vzajemna rekurze", """
def isEven(n: Int): Int =
  if n == 0 then
    return 1
  else
    val m = n - 1
    return isOdd(m)

def isOdd(n: Int): Int =
  if n == 0 then
    return 0
  else
    val m = n - 1
    return isEven(m)

@main def main(): Unit =
  print(isEven(10))
  print(isOdd(7))
  print("\\n")
""", None, None),
    "deep_recursion": ("hluboka rekurze (1000 ramcu)", """
def depth(n: Int): Int =
  if n == 0 then
    return 0
  else
    val m = n - 1
    val d = depth(m)
    return d + 1

@main def main(): Unit =
  print(depth(1000))
  print("\\n")
""", None, None),
    "implicit_result": ("vysledek funkce je posledni vyraz tela", """
def last(x: Int): Int =
  val y = x * 2
  y + 1

def single(x: Int) = x - 1

@main def main(): Unit =
  print(last(5))
  print(" ")
  print(single(5))
  print("\\n")
""", None, None),
    "unit_function_discards": ("funkce s typem Unit vrati () i pri return s hodnotou", """
def ignore(): Unit =
  return 5

def ignore2(): Unit = 7

@main def main(): Unit =
  val a = ignore()
  val b = ignore2()
  print(a)
  print(b)
  print("\\n")
""", None, None),
    "return_from_loop": ("return z cyklu a samostatny return", """
def find(limit: Int): Int =
  var i = 0
  while i < limit do
    if i * i > 20 then
      return i
    else
      i = i + 1
  0 - 1

def early(x: Int): Unit =
  if x < 0 then
    print("neg")
    return
  else
    print("pos")
  print("!")

@main def main(): Unit =
  print(find(100))
  print(" ")
  print(find(3))
  print(" ")
  early(-1)
  early(1)
  print("\\n")
""", None, None),
    "argument_order": ("FUNEXP: argumenty se vyhodnocuji zleva doprava", """
def tag(s: String, v: Int): Int =
  print(s)
  v

def sum3(a: Int, b: Int, c: Int): Int = a * 100 + b * 10 + c

@main def main(): Unit =
  val r = sum3(tag("a", 1), tag("b", 2), tag("c", 3))
  print(" ")
  print(r)
  print("\\n")
""", None, None),
    "nested_calls": ("FUNEXP: vnorena volani a volani ve vyrazech", """
def inc(x: Int): Int = x + 1

@main def main(): Unit =
  val r = 2 * inc(inc(1)) + strLength(subStr("abcd", 1, 3))
  print(toStr(r) + "\\n")
""", None, None),
    "string_building": ("dlouhy retezec skladany v cyklu", """
@main def main(): Unit =
  var s = ""
  var i = 0
  while i < 50 do
    s = s + chr(97 + i - i / 26 * 26)
    i = i + 1
  print(s)
  print(" ")
  print(strLength(s))
  print("\\n")
""", None, None),
}


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    for old in OUT.glob("*"):
        if old.suffix in (".scala", ".in", ".code"):
            old.unlink()
    for name, (desc, source, stdin, code) in PROGRAMS.items():
        (OUT / f"{name}.scala").write_text(f"// {desc}\n" + PROLOG + source.lstrip("\n"), encoding="ascii")
        if stdin is not None:
            (OUT / f"{name}.in").write_text(stdin, encoding="ascii")
        if code is not None:
            (OUT / f"{name}.code").write_text(f"{code}\n", encoding="ascii")
    print(f"{len(PROGRAMS)} programu v {OUT}")


if __name__ == "__main__":
    main()
