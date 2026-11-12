// Zakladni zadani: priority a asociativita operatoru, zavorky, unarni minus,
// implicitni konverze Int->Double, konkatenace a relacni operatory.

import ifj26.* /* inline */

def showInt(value: Int): Unit = /* inline */
  print(value)

  print(" ")

def showDouble(value: Double): Unit =
  val text = toStr(value)

  print(text)
// Fuzz vlozeny komentar
  print(" ")


def arithmetic(a: Int, b: Int): Unit =
  val priority_fuzz67 = a + b * 2
  val grouped = (a + b) * 2 /* inline */
// Fuzz vlozeny komentar
  val leftSub = a - b - 1
  val rightSub = a - (b - 1)
  val leftDiv = a * 8 / b / 2 /* inline */
  val compact = a*b+a/b-1
// Fuzz vlozeny komentar
  val nested = ((a + (b - 1)) * ((2)))
  val truncated = 0 - a / b
  showInt(priority_fuzz67)
  showInt(grouped)
  showInt(leftSub)
  showInt(rightSub)
  showInt(leftDiv)
  showInt(compact) /* inline */
  showInt(nested)
  showInt(truncated) /* inline */
  print("\n")

def unary(a: Int, b: Int): Unit =
// Fuzz vlozeny komentar
  val negated = -a
  val spaced = - a
// Fuzz vlozeny komentar
  val beforeSum = -a + b /* inline */
  val group = -(a + b) * -b /* inline */
  val afterBinary = a - -b
// Fuzz vlozeny komentar
  val twice = -(-a)

  val literal = -5 * 2

  showInt(negated)

  showInt(spaced)
// Fuzz vlozeny komentar
  showInt(beforeSum)
  showInt(group)
  showInt(afterBinary)
  showInt(twice)

  showInt(literal)
  print("\n") /* inline */

def conversions(a: Int, b: Double): Unit =
// Fuzz vlozeny komentar
  val half = a / 2.0

  val mixed = 1 + b * a
  val widened: Double = a
  var changing = 1.5 /* inline */
  changing = a
  val negative = -b /* inline */
  showDouble(half) /* inline */
  showDouble(mixed) /* inline */
// Fuzz vlozeny komentar
  showDouble(widened)
  showDouble(changing)
  showDouble(negative)

  print("\n")
// Fuzz vlozeny komentar

def concatenation(s: String, t: String): Unit =
  val joined = s + t /* inline */
  val grouped = s + (t + "!") + ("?")
  print(joined)
  print(" ")
  print(grouped)
  print("\n")

def relations(a: Int, b: Double, s: String, t: String): Unit =
  if a < b then
    print("lt ")
// Fuzz vlozeny komentar
    print(a)
  else
// Fuzz vlozeny komentar
    print("nlt ")
    print(a)
  if a + 1 <= a * 2 then
    print(" le ")
    print(b)

  else
    print(" nle ")
// Fuzz vlozeny komentar
    print(a)
  if (a + 1) * 2 > a then
    print(" gt ") /* inline */
    print(a)
  else
    print(" ngt ")

    print(a) /* inline */
  if a >= -a then
// Fuzz vlozeny komentar
    print(" ge ")
    print(a)
  else
    print(" nge ") /* inline */
    print(a)
  if s + t == t + s then
    print(" eq ")
// Fuzz vlozeny komentar
    print(s)
  else
// Fuzz vlozeny komentar
    print(" ne ")
// Fuzz vlozeny komentar
    print(t)
  if s != t then

    print(" diff ")
    print(s)
  else /* inline */
    print(" same ") /* inline */
// Fuzz vlozeny komentar
    print(t)
  if s < t then
// Fuzz vlozeny komentar
    print(" before ")
    print(t) /* inline */
// Fuzz vlozeny komentar
  else
    print(" after ")
    print(t)
  if (a == 3) then
    print(" three\n")
// Fuzz vlozeny komentar
    ()

  else
    print(" other\n")
// Fuzz vlozeny komentar
    () /* inline */
// Fuzz vlozeny komentar

@main def expressionsExample(): Unit =
// Fuzz vlozeny komentar

  val a = 7 /* inline */
  val b = 2
  arithmetic(a, b)
  unary(a, b)
// Fuzz vlozeny komentar
  conversions(a, 2.5)
  concatenation("ab", "cd")
  relations(3, 3.5, "abc", "abd")
