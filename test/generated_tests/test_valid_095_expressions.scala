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
// Fuzz vlozeny komentar

def arithmetic(a: Int, b: Int): Unit =
  val priority = a + b * 2
// Fuzz vlozeny komentar
  val grouped = (a + b) * 2
  val leftSub = a - b - 1 /* inline */

  val rightSub = a - (b - 1)
  val leftDiv = a * 8 / b / 2
  val compact = a*b+a/b-1 /* inline */
  val nested = ((a + (b - 1)) * ((2))) /* inline */
  val truncated = 0 - a / b /* inline */
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
// Fuzz vlozeny komentar
  val afterBinary = a - -b
  val twice = -(-a)
  val literal = -5 * 2
// Fuzz vlozeny komentar
  showInt(negated)
  showInt(spaced)
  showInt(beforeSum)
  showInt(group)
// Fuzz vlozeny komentar
  showInt(afterBinary)
// Fuzz vlozeny komentar

  showInt(twice)
// Fuzz vlozeny komentar
  showInt(literal)
  print("\n")
// Fuzz vlozeny komentar

def conversions(a: Int, b: Double): Unit = /* inline */
  val half = a / 2.0
  val mixed = 1 + b * a
  val widened: Double = a /* inline */

  var changing = 1.5
  changing = a
  val negative = -b
  showDouble(half)
  showDouble(mixed)
  showDouble(widened)
  showDouble(changing) /* inline */
  showDouble(negative)

  print("\n")

// Fuzz vlozeny komentar
def concatenation(s: String, t: String): Unit =
  val joined = s + t
  val grouped = s + (t + "!") + ("?")
  print(joined)
  print(" ") /* inline */
  print(grouped)
  print("\n")

// Fuzz vlozeny komentar
def relations(a: Int, b: Double, s: String, t: String): Unit =
  if a < b then
// Fuzz vlozeny komentar

    print("lt ")

    print(a) /* inline */
  else
    print("nlt ") /* inline */
    print(a)

  if a + 1 <= a * 2 then
    print(" le ")
    print(b) /* inline */
  else
    print(" nle ")
    print(a)
  if (a + 1) * 2 > a then
    print(" gt ")
    print(a)
  else
    print(" ngt ")
    print(a)
// Fuzz vlozeny komentar
  if a >= -a then
    print(" ge ")
    print(a)

  else
    print(" nge ")
    print(a)
  if s + t == t + s then
// Fuzz vlozeny komentar
    print(" eq ")
    print(s)
// Fuzz vlozeny komentar
  else /* inline */
    print(" ne ")
// Fuzz vlozeny komentar
    print(t)
  if s != t then
    print(" diff ")
    print(s)
  else /* inline */

    print(" same ")
    print(t) /* inline */

  if s < t then
    print(" before ") /* inline */
    print(t)
// Fuzz vlozeny komentar
  else
    print(" after ")
    print(t)
  if (a == 3) then
// Fuzz vlozeny komentar
    print(" three\n")
    ()
  else
    print(" other\n") /* inline */
    ()

@main def expressionsExample(): Unit =
  val a = 7
  val b = 2
// Fuzz vlozeny komentar
  arithmetic(a, b)
  unary(a, b) /* inline */
  conversions(a, 2.5)
// Fuzz vlozeny komentar
  concatenation("ab", "cd")
  relations(3, 3.5, "abc", "abd")
