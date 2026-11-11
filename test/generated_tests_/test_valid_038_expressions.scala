// Zakladni zadani: priority a asociativita operatoru, zavorky, unarni minus,
// implicitni konverze Int->Double, konkatenace a relacni operatory.
import ifj26.*

def showInt(value: Int): Unit =
  print(value)
  print(" ")

def showDouble(value: Double): Unit =
  val text = toStr(value)
  print(text)
  print(" ")

def arithmetic(a: Int, b: Int): Unit =
  val priority = a + b * 2
  val grouped = (a + b) * 2
  val leftSub = a - b - 1
  val rightSub = a - (b - 1)
  val leftDiv = a * 8 / b / 2
  val compact = a*b+a/b-1
  val nested = ((a + (b - 1)) * ((2)))
  val truncated = 0 - a / b
  showInt(priority)
  showInt(grouped)
  showInt(leftSub)
  showInt(rightSub)
  showInt(leftDiv)
  showInt(compact)
  showInt(nested)
  showInt(truncated)
  print("\n")

def unary(a: Int, b: Int): Unit =
  val negated = -a
  val spaced = - a
  val beforeSum = -a + b
  val group = -(a + b) * -b
  val afterBinary = a - -b
  val twice = -(-a)
  val literal = -5 * 2
  showInt(negated)
  showInt(spaced)
  showInt(beforeSum)
  showInt(group)
  showInt(afterBinary)
  showInt(twice)
  showInt(literal)
  print("\n")

def conversions(a: Int, b: Double): Unit =
  val half = a / 2.0
  val mixed = 1 + b * a
  val widened: Double = a
  var changing_fuzz60 = 1.5
  changing_fuzz60 = a
  val negative = -b
  showDouble(half)
  showDouble(mixed)
  showDouble(widened)
  showDouble(changing_fuzz60)
  showDouble(negative)
  print("\n")

def concatenation(s: String, t: String): Unit =
  val joined = s + t
  val grouped = s + (t + "!") + ("?")
  print(joined)
  print(" ")
  print(grouped)
  print("\n")

def relations(a: Int, b: Double, s: String, t: String): Unit =
  if a < b then
    print("lt ")
    print(a)
  else
    print("nlt ")
    print(a)
  if a + 1 <= a * 2 then
    print(" le ")
    print(b)
  else
    print(" nle ")
    print(a)
  if (a + 1) * 2 > a then
    print(" gt ")
    print(a)
  else
    print(" ngt ")
    print(a)
  if a >= -a then
    print(" ge ")
    print(a)
  else
    print(" nge ")
    print(a)
  if s + t == t + s then
    print(" eq ")
    print(s)
  else
    print(" ne ")
    print(t)
  if s != t then
    print(" diff ")
    print(s)
  else
    print(" same ")
    print(t)
  if s < t then
    print(" before ")
    print(t)
  else
    print(" after ")
    print(t)
  if (a == 3) then
    print(" three\n")
    ()
  else
    print(" other\n")
    ()

@main def expressionsExample(): Unit =
  val a = 7
  val b = 2
  arithmetic(a, b)
  unary(a, b)
  conversions(a, 2.5)
  concatenation("ab", "cd")
  relations(3, 3.5, "abc", "abd")
