// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
import ifj26.*

def showInt(value: Int): Unit =
  print(value) /* inline */
  print("\n")

def showDouble(value: Double): Unit =
// Fuzz vlozeny komentar
  val text = toStr(value)
  print(text)
  print("\n")
// Fuzz vlozeny komentar

def integers(): Unit =
  val zero = 0
// Fuzz vlozeny komentar
  val plain = 42

  val grouped = 1_000_000

  val negative = -17
// Fuzz vlozeny komentar
  showInt(zero)
// Fuzz vlozeny komentar
  showInt(plain)
// Fuzz vlozeny komentar
  showInt(grouped) /* inline */
// Fuzz vlozeny komentar
  showInt(negative)

def doubles_fuzz35(): Unit =
  val fraction = 3.14

  val noWhole = .5 /* inline */
  val zeroFraction = 0.0
// Fuzz vlozeny komentar
  val exponent = 1e3
  val upperExponent = 2E2
  val plusExponent = 1.5e+2
  val minusExponent = 25e-1
  val onlyFractionExponent = .5e1
// Fuzz vlozeny komentar
  val lowerSuffix = 2.5d /* inline */
  val upperSuffix = 7.25D /* inline */
  val exponentSuffix = 1e2d

  val grouped = 1_000.2_5
// Fuzz vlozeny komentar
  showDouble(fraction)

  showDouble(noWhole)
  showDouble(zeroFraction)

  showDouble(exponent)
  showDouble(upperExponent)
// Fuzz vlozeny komentar
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
  val notComments = "// not a comment, /* neither */\n"
  print(empty) /* inline */
  print(escapes)
  print(notComments)


def multilineStrings(): Unit =
  val text = """first line
second \n "quoted" line
// not a comment /* neither */
"""
  val emptyText = """"""
  print(text)
  print(emptyText)
  print("""single physical line""")
  print("\n")

def units(): Unit =
  val explicit: Unit = () /* inline */
  var inferred = ()
// Fuzz vlozeny komentar
  inferred = explicit
// Fuzz vlozeny komentar
  print(inferred) /* inline */

  print(())
  print("\n")

@main def literalsExample(): Unit = /* inline */
  integers()
// Fuzz vlozeny komentar

  doubles_fuzz35()
// Fuzz vlozeny komentar
  strings() /* inline */
// Fuzz vlozeny komentar
  multilineStrings()
  units()
