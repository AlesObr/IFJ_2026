// Fuzz vlozeny komentar
// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
// Fuzz vlozeny komentar
import ifj26.* /* inline */

def showInt(value: Int): Unit =
  print(value) /* inline */
  print("\n")

def showDouble(value: Double): Unit = /* inline */
  val text = toStr(value)
  print(text)
  print("\n")

def integers(): Unit = /* inline */
// Fuzz vlozeny komentar
  val zero = 0
  val plain = 42
  val grouped = 1_000_000
  val negative = -17
  showInt(zero)
  showInt(plain)
  showInt(grouped) /* inline */
// Fuzz vlozeny komentar
  showInt(negative) /* inline */

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
  val lowerSuffix = 2.5d
  val upperSuffix = 7.25D
  val exponentSuffix = 1e2d
  val grouped = 1_000.2_5
  showDouble(fraction)
  showDouble(noWhole)
  showDouble(zeroFraction)
  showDouble(exponent)
  showDouble(upperExponent) /* inline */
  showDouble(plusExponent)
  showDouble(minusExponent)
  showDouble(onlyFractionExponent)
  showDouble(lowerSuffix)
  showDouble(upperSuffix)
  showDouble(exponentSuffix)

  showDouble(grouped)

def strings(): Unit = /* inline */
  val empty = ""
  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n"
  val notComments_fuzz39 = "// not a comment, /* neither */\n"
  print(empty)
  print(escapes)
  print(notComments_fuzz39)

def multilineStrings(): Unit =
  val text = """first line
second \n "quoted" line
// not a comment /* neither */
"""
  val emptyText = """"""
  print(text)
  print(emptyText)
  print("""single physical line""")
// Fuzz vlozeny komentar
  print("\n")


def units(): Unit =

  val explicit: Unit = ()
  var inferred = ()
// Fuzz vlozeny komentar
  inferred = explicit
  print(inferred)
  print(())
// Fuzz vlozeny komentar
  print("\n")

@main def literalsExample(): Unit =
  integers()

  doubles()

  strings()
  multilineStrings()
// Fuzz vlozeny komentar
  units()
