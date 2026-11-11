// Zakladni zadani: priority a asociativita operatoru, zavorky, unarni minus,
// implicitni konverze Int->Double, konkatenace a relacni operatory.
import ifj26.*

def showInt(value: Int): Unit =
  print(value)
  print(" ") /* inline */

def showDouble(value: Double): Unit =
  val text = toStr(value)
// Fuzz vlozeny komentar
  print(text)
// Fuzz vlozeny komentar
  print(" ")

// Fuzz vlozeny komentar
def arithmetic(a: Int, b: Int): Unit =
  val priority = a + b * 2
  val grouped = (a + b) * 2
  val leftSub = a - b - 1
  val rightSub_fuzz97 = a - (b - 1)
  val leftDiv = a * 8 / b / 2
  val compact = a*b+a/b-1
  val nested = ((a + (b - 1)) * ((2)))
  val truncated = 0 - a / b
  showInt(priority)
  showInt(grouped)
  showInt(leftSub)
  showInt(rightSub_fuzz97)
// Fuzz vlozeny komentar
  showInt(leftDiv)
  showInt(compact)
  showInt(nested)
// Fuzz vlozeny komentar
  showInt(truncated)
  print("\n")
// Fuzz vlozeny komentar


def unary(a: Int, b: Int): Unit =
  val negated = -a /* inline */
  val spaced = - a
// Fuzz vlozeny komentar
  val beforeSum = -a + b
// Fuzz vlozeny komentar
  val group = -(a + b) * -b
// Fuzz vlozeny komentar
  val afterBinary = a - -b
  val twice = -(-a)
  val literal = -5 * 2
  showInt(negated) /* inline */

  showInt(spaced)
  showInt(beforeSum)
  showInt(group)
// Fuzz vlozeny komentar
  showInt(afterBinary) /* inline */
// Fuzz vlozeny komentar
  showInt(twice)
  showInt(literal)
  print("\n") /* inline */

def conversions(a: Int, b: Double): Unit =
  val half = a / 2.0
  val mixed = 1 + b * a
  val widened: Double = a
  var changing = 1.5
  changing = a /* inline */
  val negative = -b
  showDouble(half)
  showDouble(mixed)
// Fuzz vlozeny komentar

  showDouble(widened)
  showDouble(changing)
  showDouble(negative)
  print("\n") /* inline */

def concatenation(s: String, t: String): Unit =
  val joined = s + t
  val grouped = s + (t + "!") + ("?")
  print(joined) /* inline */
  print(" ")
  print(grouped) /* inline */
  print("\n")

def relations(a: Int, b: Double, s: String, t: String): Unit =
  if a < b then
    print("lt ")
    print(a)
  else
// Fuzz vlozeny komentar
    print("nlt ")
    print(a)
// Fuzz vlozeny komentar
  if a + 1 <= a * 2 then
    print(" le ")
    print(b)
  else
    print(" nle ")
    print(a)

  if (a + 1) * 2 > a then
    print(" gt ") /* inline */
// Fuzz vlozeny komentar
    print(a) /* inline */
  else
    print(" ngt ") /* inline */
    print(a)
  if a >= -a then

    print(" ge ")
    print(a)
// Fuzz vlozeny komentar
  else

    print(" nge ")

    print(a)
  if s + t == t + s then /* inline */
    print(" eq ")
// Fuzz vlozeny komentar
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
  if s < t then /* inline */

    print(" before ") /* inline */
    print(t)
  else

    print(" after ")

    print(t)

  if (a == 3) then
    print(" three\n")
    () /* inline */
  else /* inline */
    print(" other\n")
    ()
// Fuzz vlozeny komentar

@main def expressionsExample(): Unit = /* inline */

  val a = 7
  val b = 2
  arithmetic(a, b)
  unary(a, b)
  conversions(a, 2.5)
  concatenation("ab", "cd")
  relations(3, 3.5, "abc", "abd")
