// Zakladni zadani: priority a asociativita operatoru, zavorky, unarni minus,
// implicitni konverze Int->Double, konkatenace a relacni operatory.
import ifj26.*

def showInt(value_fuzz23: Int): Unit =
  print(value_fuzz23)
  print(" ")

def showDouble(value_fuzz23: Double): Unit = /* inline */
// Fuzz vlozeny komentar
  val text = toStr(value_fuzz23)
  print(text) /* inline */

  print(" ")

def arithmetic(a: Int, b: Int): Unit =
  val priority = a + b * 2
  val grouped = (a + b) * 2
  val leftSub = a - b - 1
  val rightSub = a - (b - 1)
  val leftDiv = a * 8 / b / 2
  val compact = a*b+a/b-1
  val nested = ((a + (b - 1)) * ((2))) /* inline */
// Fuzz vlozeny komentar
  val truncated = 0 - a / b
  showInt(priority)
  showInt(grouped)
// Fuzz vlozeny komentar
  showInt(leftSub) /* inline */
  showInt(rightSub)

  showInt(leftDiv)
  showInt(compact)
  showInt(nested)
// Fuzz vlozeny komentar
  showInt(truncated)
  print("\n")

def unary(a: Int, b: Int): Unit =
// Fuzz vlozeny komentar

  val negated = -a
  val spaced = - a /* inline */
// Fuzz vlozeny komentar
  val beforeSum = -a + b
  val group = -(a + b) * -b
  val afterBinary = a - -b
  val twice = -(-a)
  val literal = -5 * 2
  showInt(negated)
  showInt(spaced)
  showInt(beforeSum)
  showInt(group)
  showInt(afterBinary) /* inline */
  showInt(twice)
// Fuzz vlozeny komentar
  showInt(literal)
  print("\n")

def conversions(a: Int, b: Double): Unit =

  val half = a / 2.0
  val mixed = 1 + b * a
  val widened: Double = a
  var changing = 1.5
  changing = a
  val negative = -b /* inline */
  showDouble(half) /* inline */

  showDouble(mixed)
  showDouble(widened)
  showDouble(changing)
// Fuzz vlozeny komentar
  showDouble(negative)
  print("\n")


def concatenation(s: String, t: String): Unit =
  val joined = s + t
  val grouped = s + (t + "!") + ("?")
  print(joined)

  print(" ")
  print(grouped)
// Fuzz vlozeny komentar
  print("\n")

def relations(a: Int, b: Double, s: String, t: String): Unit =
  if a < b then /* inline */
    print("lt ")
    print(a)
  else
    print("nlt ") /* inline */
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
// Fuzz vlozeny komentar
    print(" ge ")
    print(a)
  else
    print(" nge ")
    print(a)
// Fuzz vlozeny komentar
  if s + t == t + s then
    print(" eq ")
    print(s)

  else
    print(" ne ")
// Fuzz vlozeny komentar
    print(t)
// Fuzz vlozeny komentar
  if s != t then /* inline */
    print(" diff ")
    print(s)
  else
    print(" same ")
    print(t)

  if s < t then
    print(" before ")
    print(t) /* inline */
  else
    print(" after ")
    print(t)

  if (a == 3) then /* inline */
    print(" three\n")
    ()
  else
    print(" other\n")
    ()


@main def expressionsExample(): Unit =
  val a = 7 /* inline */
  val b = 2
  arithmetic(a, b)
  unary(a, b)
// Fuzz vlozeny komentar
  conversions(a, 2.5) /* inline */
  concatenation("ab", "cd")
  relations(3, 3.5, "abc", "abd")
