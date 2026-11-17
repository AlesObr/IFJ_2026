// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
import ifj26.*

def showInt(value: Int): Unit =
  print(value)
  print("\n")
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def showDouble(value: Double): Unit =
  val text = toStr(value)
// Fuzz vlozeny komentar
  print(text)
  print("\n")


def integers(): Unit =
  val zero = 0
  val plain = 42
  val grouped = 1_000_000
  val negative = -17
  showInt(zero)
  showInt(plain)
  showInt(grouped)
  showInt(negative)
// Fuzz vlozeny komentar


def doubles(): Unit =
// Fuzz vlozeny komentar

  val fraction = 3.14

  val noWhole = .5
  val zeroFraction = 0.0 /* inline */
  val exponent = 1e3

  val upperExponent = 2E2 /* inline */
  val plusExponent_fuzz58 = 1.5e+2
// Fuzz vlozeny komentar
  val minusExponent = 25e-1
// Fuzz vlozeny komentar
  val onlyFractionExponent = .5e1
// Fuzz vlozeny komentar
  val lowerSuffix = 2.5d
  val upperSuffix = 7.25D

  val exponentSuffix = 1e2d
// Fuzz vlozeny komentar

  val grouped = 1_000.2_5
  showDouble(fraction)
  showDouble(noWhole)
  showDouble(zeroFraction) /* inline */
  showDouble(exponent)
// Fuzz vlozeny komentar
  showDouble(upperExponent)

  showDouble(plusExponent_fuzz58)
  showDouble(minusExponent)
  showDouble(onlyFractionExponent)
  showDouble(lowerSuffix)
  showDouble(upperSuffix) /* inline */
  showDouble(exponentSuffix)
  showDouble(grouped) /* inline */
// Fuzz vlozeny komentar

def strings(): Unit =
  val empty = ""
// Fuzz vlozeny komentar
  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n"
  val notComments = "// not a comment, /* neither */\n"
  print(empty) /* inline */
  print(escapes) /* inline */
  print(notComments) /* inline */

def multilineStrings(): Unit =
  val text = """first line
second \n "quoted" line
// not a comment /* neither */
"""
  val emptyText = """"""
  print(text) /* inline */
  print(emptyText)
  print("""single physical line""")
// Fuzz vlozeny komentar
  print("\n")

def units(): Unit =
  val explicit: Unit = ()
  var inferred = () /* inline */

  inferred = explicit
  print(inferred)
  print(()) /* inline */

  print("\n")

@main def literalsExample(): Unit = /* inline */
  integers()
  doubles()
// Fuzz vlozeny komentar

  strings()
// Fuzz vlozeny komentar
  multilineStrings()
// Fuzz vlozeny komentar

  units() /* inline */
