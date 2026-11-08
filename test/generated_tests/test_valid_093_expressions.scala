// Zakladni zadani: priority a asociativita operatoru, zavorky, unarni minus,
// implicitni konverze Int->Double, konkatenace a relacni operatory. /* inline */
import ifj26.*

def showInt(value: Int): Unit =
  print(value)
// Fuzz vlozeny komentar
  print(" ")


def showDouble(value: Double): Unit =
  val text = toStr(value)
  print(text)
  print(" ")

def arithmetic(a: Int, b: Int): Unit = /* inline */

  val priority = a + b * 2
  val grouped = (a + b) * 2 /* inline */
  val leftSub = a - b - 1
  val rightSub = a - (b - 1)
  val leftDiv = a * 8 / b / 2
  val compact = a*b+a/b-1 /* inline */
  val nested = ((a + (b - 1)) * ((2)))
  val truncated = 0 - a / b
// Fuzz vlozeny komentar
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
// Fuzz vlozeny komentar
  showInt(twice) /* inline */
  showInt(literal)
  print("\n") /* inline */

// Fuzz vlozeny komentar
def conversions(a: Int, b: Double): Unit =
// Fuzz vlozeny komentar
  val half = a / 2.0
  val mixed = 1 + b * a
  val widened: Double = a /* inline */
  var changing = 1.5 /* inline */

  changing = a
  val negative = -b /* inline */
  showDouble(half)

  showDouble(mixed)
  showDouble(widened) /* inline */
  showDouble(changing)
  showDouble(negative)
  print("\n")

def concatenation(s: String, t: String): Unit = /* inline */
  val joined = s + t
  val grouped = s + (t + "!") + ("?")
  print(joined)
  print(" ")
  print(grouped)
  print("\n")
// Fuzz vlozeny komentar

def relations(a: Int, b: Double, s: String, t: String): Unit =
  if a < b then
    print("lt ")
    print(a)
  else /* inline */
// Fuzz vlozeny komentar
    print("nlt ")
    print(a) /* inline */
  if a + 1 <= a * 2 then
    print(" le ")
    print(b)
// Fuzz vlozeny komentar
  else
// Fuzz vlozeny komentar
    print(" nle ")
    print(a)
  if (a + 1) * 2 > a then
    print(" gt ")
    print(a)

  else
    print(" ngt ")
    print(a) /* inline */
  if a >= -a then
    print(" ge ")
    print(a)
// Fuzz vlozeny komentar
  else
    print(" nge ")
    print(a)
// Fuzz vlozeny komentar
  if s + t == t + s then
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
// Fuzz vlozeny komentar
    print(" same ")
// Fuzz vlozeny komentar
    print(t)
  if s < t then
    print(" before ")
    print(t)

  else
    print(" after ")
    print(t) /* inline */
  if (a == 3) then
// Fuzz vlozeny komentar
    print(" three\n")

    () /* inline */
  else
    print(" other\n")
    ()

@main def expressionsExample(): Unit =
  val a = 7
  val b = 2
  arithmetic(a, b)
  unary(a, b)
  conversions(a, 2.5) /* inline */
  concatenation("ab", "cd")
  relations(3, 3.5, "abc", "abd") /* inline */
