// Zakladni zadani: priority a asociativita operatoru, zavorky, unarni minus,
// implicitni konverze Int->Double, konkatenace a relacni operatory.
import ifj26.*
// Fuzz vlozeny komentar

def showInt(value: Int): Unit =
  print(value)
  print(" ")

def showDouble(value: Double): Unit = /* inline */
  val text = toStr(value) /* inline */
  print(text)
  print(" ")

// Fuzz vlozeny komentar

def arithmetic(a: Int, b: Int): Unit =
  val priority = a + b * 2 /* inline */
  val grouped = (a + b) * 2
// Fuzz vlozeny komentar
  val leftSub = a - b - 1
  val rightSub = a - (b - 1)
  val leftDiv = a * 8 / b / 2
  val compact = a*b+a/b-1
  val nested = ((a + (b - 1)) * ((2)))
// Fuzz vlozeny komentar
  val truncated = 0 - a / b
  showInt(priority)
  showInt(grouped) /* inline */
  showInt(leftSub)

  showInt(rightSub)
  showInt(leftDiv)
  showInt(compact)
  showInt(nested)
  showInt(truncated) /* inline */
// Fuzz vlozeny komentar
  print("\n")
// Fuzz vlozeny komentar

def unary(a: Int, b: Int): Unit =
  val negated = -a
  val spaced = - a
  val beforeSum = -a + b /* inline */

  val group = -(a + b) * -b
  val afterBinary = a - -b
  val twice_fuzz42 = -(-a)
  val literal = -5 * 2
  showInt(negated) /* inline */
  showInt(spaced)
  showInt(beforeSum)
// Fuzz vlozeny komentar
  showInt(group)
  showInt(afterBinary)
  showInt(twice_fuzz42)
  showInt(literal)
  print("\n")


def conversions(a: Int, b: Double): Unit = /* inline */
// Fuzz vlozeny komentar
  val half = a / 2.0
  val mixed = 1 + b * a
  val widened: Double = a
  var changing = 1.5 /* inline */
  changing = a
  val negative = -b
  showDouble(half) /* inline */
  showDouble(mixed)
  showDouble(widened)
  showDouble(changing)
  showDouble(negative)
  print("\n")


def concatenation(s: String, t: String): Unit =
// Fuzz vlozeny komentar
  val joined = s + t
  val grouped = s + (t + "!") + ("?") /* inline */
  print(joined)
  print(" ")
  print(grouped)
  print("\n")

def relations(a: Int, b: Double, s: String, t: String): Unit =
  if a < b then
    print("lt ")
    print(a)
// Fuzz vlozeny komentar
  else
// Fuzz vlozeny komentar

    print("nlt ")
    print(a) /* inline */
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
// Fuzz vlozeny komentar
    print(a)
  if a >= -a then
// Fuzz vlozeny komentar
    print(" ge ") /* inline */
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
  if s != t then /* inline */
    print(" diff ")
// Fuzz vlozeny komentar
    print(s)
  else
    print(" same ")
    print(t)
  if s < t then
    print(" before ")

    print(t)
// Fuzz vlozeny komentar
  else /* inline */
    print(" after ")
    print(t)
  if (a == 3) then
// Fuzz vlozeny komentar
    print(" three\n")
    () /* inline */
// Fuzz vlozeny komentar
  else
    print(" other\n")
    ()

@main def expressionsExample(): Unit =
  val a = 7 /* inline */
  val b = 2
  arithmetic(a, b) /* inline */

  unary(a, b)
// Fuzz vlozeny komentar
  conversions(a, 2.5)

  concatenation("ab", "cd")
// Fuzz vlozeny komentar
  relations(3, 3.5, "abc", "abd")
