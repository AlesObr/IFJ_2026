#!/usr/bin/env python3
"""
Cilene testy rozsireni: jeden maly program na jednu konstrukci, nazev <rozsireni>_<konstrukce>.

Zapise do extensions/ soubory .scala (a .in, je-li vstup). Ocekavany vystup .out vytvori
scala_expected.py (Scala s ifj26.scala). Stav podle faze ukaze ext_status.py.

Pouziti: python3 make_ext_programs.py && python3 scala_expected.py --dir extensions
"""

from pathlib import Path

OUT = Path(__file__).resolve().parent / "extensions"
PROLOG = "import ifj26.*\n\n"

# name -> (popis dle zadani, program bez prologu, vstup nebo None)
PROGRAMS = {
    # ------------------------------------------------------------- FUNEXP (5.1)
    "funexp_nested_calls": ("volani vnorena a ve vyrazu", """
def inc(x: Int): Int = x + 1

@main def main(): Unit =
  val r = 2 * inc(inc(1)) + 1
  print(r)
""", None),
    "funexp_call_as_argument": ("vyraz a volani jako argument", """
def add(a: Int, b: Int): Int = a + b

@main def main(): Unit =
  print(add(add(1, 2) * 2, strLength("abc")))
""", None),
    "funexp_call_in_condition": ("volani v podmince", """
def two(): Int = 2

@main def main(): Unit =
  if two() + 1 > 2 then
    print("yes")
  else
    print("no")
""", None),

    # ------------------------------------------------------------- BOOLTHEN (5.2)
    "boolthen_literals": ("literaly true/false a print", """
@main def main(): Unit =
  print(true)
  print(" ")
  print(false)
""", None),
    "boolthen_variable_param_return": ("Boolean jako promenna, parametr a navratovy typ", """
def negate(b: Boolean): Boolean = !b

@main def main(): Unit =
  val t: Boolean = 1 < 2
  val f = negate(t)
  print(f)
""", None),
    "boolthen_operators": ("operatory !, && a ||", """
@main def main(): Unit =
  val a = true
  val b = false
  print(a && b)
  print(" ")
  print(a || b)
  print(" ")
  print(!a || !b && a)
""", None),
    "boolthen_short_circuit": ("zkratove vyhodnoceni && a ||", """
def side(s: String, v: Boolean): Boolean =
  print(s)
  v

@main def main(): Unit =
  val x = side("a", false) && side("b", true)
  val y = side("c", true) || side("d", true)
  print(" ")
  print(x)
  print(y)
""", None),
    "boolthen_condition_variable": ("Boolean promenna jako podminka if a while", """
@main def main(): Unit =
  var go = true
  var i = 0
  while go do
    i = i + 1
    go = i < 3
  if go then
    print("bad")
  else
    print(i)
""", None),
    "boolthen_if_without_else": ("zkraceny if bez else", """
@main def main(): Unit =
  val x = 5
  if x > 0 then
    print("positive")
  print("!")
""", None),
    "boolthen_else_if": ("vetve else if", """
def sign(x: Int): Unit =
  if x < 0 then
    print("neg")
  else if x == 0 then
    print("zero")
  else
    print("pos")

@main def main(): Unit =
  sign(-1)
  sign(0)
  sign(1)
""", None),
    "boolthen_equality": ("== a != nad Boolean", """
@main def main(): Unit =
  val a = true
  if a == (1 < 2) then
    print("eq")
  else
    print("ne")
  if a != false then
    print("ne")
  else
    print("eq")
""", None),
    "boolthen_tostr": ("toStr pro Boolean", """
@main def main(): Unit =
  val s = toStr(false)
  print(s + "|" + toStr(true))
""", None),
    "boolthen_readboolean": ("vestavena funkce readBoolean", """
@main def main(): Unit =
  val a = readBoolean()
  val b = readBoolean()
  print(a)
  print(b)
""", "true\nfalse\n"),

    # ------------------------------------------------------------- CONTROL (5.3)
    "control_if_expression": ("if jako vyraz na jednom radku", """
@main def main(): Unit =
  val x = -5
  val absolute = if x < 0 then 0 - x else x
  print(absolute)
""", None),
    "control_if_expression_blocks": ("if jako vyraz s bloky", """
@main def main(): Unit =
  val x = 3
  val y = if x > 2 then
    val d = x * 2
    d + 1
  else
    0
  print(y)
""", None),
    "control_if_expression_conversion": ("if vyraz s implicitni konverzi Int -> Double", """
@main def main(): Unit =
  val x = 1
  val y: Double = if x > 0 then 2 else 2.5
  print(y)
""", None),
    "control_single_line_branches": ("jednoradkova tela vetvi prikazu if", """
@main def main(): Unit =
  val x = 1
  if x > 0 then print("pos") else print("neg")
""", None),
    "control_for_to": ("for s rozsahem to (vcetne konce)", """
@main def main(): Unit =
  for i <- 1 to 5 do
    print(i)
""", None),
    "control_for_until": ("for s rozsahem until (bez konce)", """
@main def main(): Unit =
  for i <- 0 until 3 do
    print(i)
""", None),
    "control_for_by_negative": ("for se zapornym krokem by", """
@main def main(): Unit =
  for i <- 10 to 1 by -3 do
    print(i)
    print(" ")
""", None),
    "control_for_single_line": ("for s jednoradkovym telem", """
@main def main(): Unit =
  for i <- 1 to 3 do print(i * i)
""", None),
    "control_for_guards": ("for se strazemi ve viceradkovem tvaru", """
@main def main(): Unit =
  val limit = 10
  for
    i <- 0 until limit
    if i != 3
    if i < 8
  do
    print(i)
""", None),
    "control_for_bounds_once": ("meze rozsahu se vyhodnoti jen jednou", """
def bound(): Int =
  print("b")
  3

@main def main(): Unit =
  for i <- 1 to bound() do
    print(i)
""", None),

    # ------------------------------------------------------------- VALMEM (5.4)
    "valmem_tostring": ("clen toString", """
@main def main(): Unit =
  val x = 42
  val s = x.toString
  print(s + "!")
""", None),
    "valmem_double_members": ("cleny toInt a floor nad Double", """
@main def main(): Unit =
  val d = -2.5
  print(d.toInt)
  print(" ")
  print(d.floor)
""", None),
    "valmem_length": ("clen length nad String", """
@main def main(): Unit =
  val s = "hello"
  print(s.length)
""", None),
    "valmem_chaining": ("retezeni clenu", """
@main def main(): Unit =
  val number = 3.7
  print(number.floor.toInt)
  print("abc".length.toString)
""", None),

    # ------------------------------------------------------------- EXTKW (5.9)
    "extkw_parameterless_method": ("bezparametricka rozsirujici metoda", """
extension (value: Int)
  def twice: Int = value * 2

@main def main(): Unit =
  val x: Int = 6
  print(x.twice)
""", None),
    "extkw_empty_parameter_list": ("metoda s prazdnym seznamem parametru", """
extension (value: Int)
  def foo(): String = "foo" + toStr(value)

@main def main(): Unit =
  val x: Int = 6
  print(x.foo())
""", None),
    "extkw_method_with_parameter": ("metoda s parametrem", """
extension (value: Int)
  def plus(other: Int): Int = value + other

@main def main(): Unit =
  val x: Int = 6
  print(x.plus(12))
""", None),
    "extkw_string_receiver": ("rozsireni typu String", """
extension (s: String)
  def shout(): String = s + "!"

@main def main(): Unit =
  print("hey".shout())
""", None),

    # ------------------------------------------------------------- PARAMLISTS (5.10)
    "paramlists_no_parameter_list": ("funkce bez seznamu parametru volana jmenem", """
def answer: Int = 42

@main def main(): Unit =
  val x = answer
  print(x)
""", None),
    "paramlists_multiple_lists": ("vice seznamu parametru", """
def add(a: Int)(b: Int)(c: Int): Int = a + b + c

@main def main(): Unit =
  val sum = add(1)(2)(3)
  print(sum)
""", None),
    "paramlists_empty_first_list": ("prazdny prvni seznam", """
def offset()(value: Int): Int = value + 1

@main def main(): Unit =
  print(offset()(41))
""", None),
    "paramlists_reevaluated": ("funkce bez seznamu se vyhodnoti pri kazdem pouziti", """
def tick: Int =
  print("t")
  1

@main def main(): Unit =
  val a = tick
  val b = tick
  print(a + b)
""", None),

    # ------------------------------------------------------------- ALGEBDT (5.5)
    "algebdt_union_variable": ("promenna typu sjednoceni", """
type Scalar = Int | Double | String

@main def main(): Unit =
  var anything: Scalar = 5
  anything = "hello"
  print(toStr(anything))
""", None),
    "algebdt_union_parameter": ("parametr typu sjednoceni", """
def show(x: Double | String): String =
  return toStr(x)

@main def main(): Unit =
  print(show(5))
  print(show("s"))
""", None),
    "algebdt_case_class": ("datova trida, konstrukce a atributy", """
case class Point(x: Int, y: Int)

@main def main(): Unit =
  val p = Point(10, 20)
  print(p.x + p.y)
""", None),
    "algebdt_match": ("match s literalem, typovym vzorem, strazi a konstruktorem", """
case class Point(x: Int, y: Int)

def describe(value: Int | String | Point): String =
  value match
    case 0 => "zero"
    case n: Int if n > 0 => "positive"
    case text: String => text
    case Point(a, b) => "point"
    case _ => "negative"

@main def main(): Unit =
  print(describe(0) + describe(5) + describe("t") + describe(Point(1, 2)) + describe(-1))
""", None),
    "algebdt_read_maybe": ("readIntMaybe vraci () pri chybnem vstupu", """
def show(v: Int | Unit): String =
  v match
    case n: Int => toStr(n)
    case _ => "none"

@main def main(): Unit =
  print(show(readIntMaybe()))
  print(show(readIntMaybe()))
""", "12\nabc\n"),

    # ------------------------------------------------------------- LISTS (5.8)
    "lists_create_and_index": ("vytvoreni seznamu a pristup indexem", """
@main def main(): Unit =
  val lst = List(5, 6, 7)
  print(lst(1))
""", None),
    "lists_members": ("cleny drop, take, slice, head, tail", """
@main def main(): Unit =
  val lst = List(1, 2, 3, 4, 5)
  print(lst.drop(2).head)
  print(lst.take(2).tail.head)
  print(lst.slice(1, 3).head)
""", None),
    "lists_tostring": ("toString a print seznamu", """
@main def main(): Unit =
  val lst = List(1, 2, 3)
  print(lst)
  print(" " + lst.toString)
""", None),
    "lists_range_and_concat": ("List.range a operator ++", """
@main def main(): Unit =
  val a = List.range(1, 4)
  val b = List(10) ++ a
  print(b)
""", None),
    "lists_for_source": ("seznam jako zdroj cyklu for (CONTROL)", """
@main def main(): Unit =
  val lst = List(3, 1, 2)
  for x <- lst do
    print(x)
""", None),
    "lists_index_out_of_range": ("index mimo seznam konci chybou 40", """
@main def main(): Unit =
  val lst = List(1)
  print(lst(5))
""", None),
}

# Ocekavany kod interpretu jiny nez 0 (Scala konci vyjimkou, ifj26.scala List ale vola exit(40))
CODES = {"lists_index_out_of_range": 40}


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    for old in OUT.glob("*"):
        if old.suffix in (".scala", ".in", ".code"):
            old.unlink()
    for name, (desc, source, stdin) in PROGRAMS.items():
        (OUT / f"{name}.scala").write_text(f"// {desc}\n" + PROLOG + source.lstrip("\n"), encoding="ascii")
        if stdin is not None:
            (OUT / f"{name}.in").write_text(stdin, encoding="ascii")
        if name in CODES:
            (OUT / f"{name}.code").write_text(f"{CODES[name]}\n", encoding="ascii")
    print(f"{len(PROGRAMS)} programu v {OUT}")


if __name__ == "__main__":
    main()
