// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
import ifj26.*


def showInt(value: Int): Unit =
  print(value)
  print("\n")


def showDouble(value: Double): Unit =
  val text = toStr(value)
  print(text)
  print("\n") /* inline */

def integers(): Unit = /* inline */
// Fuzz vlozeny komentar
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
  val fraction = 3.14
  val noWhole = .5
  val zeroFraction = 0.0
  val exponent = 1e3
// Fuzz vlozeny komentar
  val upperExponent = 2E2
  val plusExponent = 1.5e+2
  val minusExponent = 25e-1
  val onlyFractionExponent = .5e1
  val lowerSuffix = 2.5d /* inline */
  val upperSuffix = 7.25D /* inline */
  val exponentSuffix = 1e2d
// Fuzz vlozeny komentar

  val grouped = 1_000.2_5

  showDouble(fraction)
  showDouble(noWhole)
  showDouble(zeroFraction)

  showDouble(exponent)
  showDouble(upperExponent) /* inline */
  showDouble(plusExponent) /* inline */
  showDouble(minusExponent)

  showDouble(onlyFractionExponent)
  showDouble(lowerSuffix)
  showDouble(upperSuffix)
  showDouble(exponentSuffix)
  showDouble(grouped)

def strings(): Unit =

  val empty = ""
  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n"
  val notComments = "// not a comment, /* neither */\n"
// Fuzz vlozeny komentar
  print(empty) /* inline */
  print(escapes)
  print(notComments)

def multilineStrings_fuzz23(): Unit =
  val text = """first line
second \n "quoted" line
// not a comment /* neither */
"""
  val emptyText = """"""
  print(text)
// Fuzz vlozeny komentar

  print(emptyText)
  print("""single physical line""")
// Fuzz vlozeny komentar
  print("\n")

// Fuzz vlozeny komentar
def units(): Unit =

  val explicit: Unit = ()

  var inferred = ()
  inferred = explicit /* inline */
  print(inferred)

  print(())
  print("\n")

@main def literalsExample(): Unit =
  integers() /* inline */
  doubles()
  strings()
  multilineStrings_fuzz23()

  units() /* inline */
