// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit. /* inline */
// Fuzz vlozeny komentar
import ifj26.*

def showInt(value: Int): Unit =
  print(value)

  print("\n")

def showDouble(value: Double): Unit =
  val text = toStr(value)
  print(text)
  print("\n")

def integers(): Unit = /* inline */
  val zero = 0 /* inline */
  val plain = 42
  val grouped = 1_000_000
  val negative = -17
  showInt(zero)
  showInt(plain)
  showInt(grouped)
  showInt(negative) /* inline */

def doubles(): Unit = /* inline */
  val fraction = 3.14 /* inline */
// Fuzz vlozeny komentar
  val noWhole = .5
  val zeroFraction = 0.0
  val exponent = 1e3
  val upperExponent = 2E2
// Fuzz vlozeny komentar

  val plusExponent = 1.5e+2
  val minusExponent = 25e-1
  val onlyFractionExponent = .5e1
  val lowerSuffix = 2.5d
// Fuzz vlozeny komentar
  val upperSuffix = 7.25D
  val exponentSuffix = 1e2d /* inline */
  val grouped = 1_000.2_5
// Fuzz vlozeny komentar

  showDouble(fraction)
  showDouble(noWhole) /* inline */

  showDouble(zeroFraction)

  showDouble(exponent)

  showDouble(upperExponent)
  showDouble(plusExponent)
  showDouble(minusExponent)
  showDouble(onlyFractionExponent)

  showDouble(lowerSuffix)
  showDouble(upperSuffix)
  showDouble(exponentSuffix)
  showDouble(grouped)

def strings(): Unit =
  val empty = ""

  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n"
  val notComments = "// not a comment, /* neither */\n" /* inline */
  print(empty)
  print(escapes)
  print(notComments)

def multilineStrings(): Unit =
  val text = """first line
second \n "quoted" line
// not a comment /* neither */
"""

  val emptyText = """"""
// Fuzz vlozeny komentar
  print(text) /* inline */
  print(emptyText)

  print("""single physical line""")
  print("\n")

// Fuzz vlozeny komentar
def units(): Unit =

  val explicit: Unit = ()
  var inferred = ()
  inferred = explicit
// Fuzz vlozeny komentar
  print(inferred)
  print(()) /* inline */
  print("\n")

@main def literalsExample(): Unit =
  integers()

  doubles()
  strings()
  multilineStrings() /* inline */
  units()
