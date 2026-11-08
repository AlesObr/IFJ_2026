// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
// Fuzz vlozeny komentar
import ifj26.*

def showInt(value: Int): Unit = /* inline */
  print(value)
  print("\n") /* inline */

def showDouble(value: Double): Unit =
// Fuzz vlozeny komentar

  val text = toStr(value)
  print(text)
  print("\n")

def integers(): Unit =
  val zero = 0
  val plain = 42

  val grouped = 1_000_000
  val negative = -17
  showInt(zero) /* inline */
  showInt(plain)
  showInt(grouped)
  showInt(negative)

def doubles(): Unit =
  val fraction = 3.14
  val noWhole = .5
// Fuzz vlozeny komentar
  val zeroFraction = 0.0
  val exponent = 1e3 /* inline */
  val upperExponent = 2E2
  val plusExponent = 1.5e+2
  val minusExponent = 25e-1
// Fuzz vlozeny komentar
  val onlyFractionExponent_fuzz52 = .5e1
  val lowerSuffix = 2.5d /* inline */
  val upperSuffix = 7.25D
  val exponentSuffix = 1e2d
  val grouped = 1_000.2_5
  showDouble(fraction)
  showDouble(noWhole)
  showDouble(zeroFraction)
  showDouble(exponent)
  showDouble(upperExponent)
  showDouble(plusExponent)

  showDouble(minusExponent)
  showDouble(onlyFractionExponent_fuzz52)
  showDouble(lowerSuffix)
  showDouble(upperSuffix)
  showDouble(exponentSuffix)
  showDouble(grouped)

def strings(): Unit =
  val empty = ""
// Fuzz vlozeny komentar
  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n" /* inline */
  val notComments = "// not a comment, /* neither */\n"
  print(empty)
// Fuzz vlozeny komentar
  print(escapes)
  print(notComments) /* inline */

def multilineStrings(): Unit =
  val text = """first line
second \n "quoted" line
// not a comment /* neither */
""" /* inline */
  val emptyText = """"""
  print(text)
  print(emptyText) /* inline */
  print("""single physical line""")
// Fuzz vlozeny komentar
  print("\n")

def units(): Unit =
// Fuzz vlozeny komentar
  val explicit: Unit = ()
  var inferred = ()
  inferred = explicit
  print(inferred)
  print(())

  print("\n")


@main def literalsExample(): Unit =
  integers()
  doubles() /* inline */
  strings()
  multilineStrings()
  units()
