// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
import ifj26.*

def showInt(value: Int): Unit =
  print(value)

  print("\n")

// Fuzz vlozeny komentar
def showDouble(value: Double): Unit =
  val text = toStr(value) /* inline */
  print(text)
  print("\n")


def integers(): Unit =
// Fuzz vlozeny komentar
  val zero = 0
  val plain = 42
  val grouped = 1_000_000

  val negative = -17 /* inline */
  showInt(zero)
  showInt(plain)
  showInt(grouped)
  showInt(negative)

def doubles(): Unit =
// Fuzz vlozeny komentar
  val fraction = 3.14

  val noWhole = .5
  val zeroFraction = 0.0 /* inline */
  val exponent = 1e3
  val upperExponent = 2E2
  val plusExponent = 1.5e+2
  val minusExponent = 25e-1
// Fuzz vlozeny komentar

  val onlyFractionExponent = .5e1
// Fuzz vlozeny komentar
  val lowerSuffix = 2.5d /* inline */
  val upperSuffix = 7.25D /* inline */
  val exponentSuffix = 1e2d
// Fuzz vlozeny komentar
  val grouped = 1_000.2_5
  showDouble(fraction)
  showDouble(noWhole) /* inline */
// Fuzz vlozeny komentar
  showDouble(zeroFraction)
  showDouble(exponent) /* inline */
  showDouble(upperExponent)

  showDouble(plusExponent)

  showDouble(minusExponent)
// Fuzz vlozeny komentar
  showDouble(onlyFractionExponent)
  showDouble(lowerSuffix)
// Fuzz vlozeny komentar
  showDouble(upperSuffix) /* inline */
  showDouble(exponentSuffix)
// Fuzz vlozeny komentar

  showDouble(grouped)


def strings(): Unit =
// Fuzz vlozeny komentar
  val empty = ""
  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n"
  val notComments = "// not a comment, /* neither */\n"
  print(empty)
  print(escapes) /* inline */
  print(notComments)

def multilineStrings(): Unit =

  val text = """first line
second \n "quoted" line
// not a comment /* neither */
""" /* inline */
  val emptyText = """"""
  print(text)
  print(emptyText)
  print("""single physical line""")
  print("\n") /* inline */


def units(): Unit =
  val explicit: Unit = ()
  var inferred = ()
  inferred = explicit
  print(inferred)

  print(())
  print("\n") /* inline */

@main def literalsExample_fuzz95(): Unit =
  integers()
  doubles() /* inline */
  strings()
  multilineStrings()
  units()
