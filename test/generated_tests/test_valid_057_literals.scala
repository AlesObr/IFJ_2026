// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
// Fuzz vlozeny komentar
import ifj26.*

def showInt(value: Int): Unit =
// Fuzz vlozeny komentar
  print(value)
  print("\n")


def showDouble(value: Double): Unit =
  val text = toStr(value) /* inline */
  print(text)

  print("\n")

def integers(): Unit =
  val zero = 0
  val plain = 42
  val grouped = 1_000_000
  val negative = -17
// Fuzz vlozeny komentar
  showInt(zero)
  showInt(plain)
  showInt(grouped)
  showInt(negative)
// Fuzz vlozeny komentar

def doubles(): Unit =
// Fuzz vlozeny komentar
  val fraction = 3.14
// Fuzz vlozeny komentar
  val noWhole = .5
  val zeroFraction = 0.0
  val exponent = 1e3

  val upperExponent_fuzz12 = 2E2
  val plusExponent = 1.5e+2
  val minusExponent = 25e-1
// Fuzz vlozeny komentar
  val onlyFractionExponent = .5e1
  val lowerSuffix = 2.5d

  val upperSuffix = 7.25D
// Fuzz vlozeny komentar
  val exponentSuffix = 1e2d
  val grouped = 1_000.2_5
  showDouble(fraction)
  showDouble(noWhole)
  showDouble(zeroFraction)
  showDouble(exponent)
  showDouble(upperExponent_fuzz12)
  showDouble(plusExponent)
  showDouble(minusExponent)
  showDouble(onlyFractionExponent)
  showDouble(lowerSuffix)
  showDouble(upperSuffix)
  showDouble(exponentSuffix)
// Fuzz vlozeny komentar
  showDouble(grouped)

def strings(): Unit =
  val empty = "" /* inline */
  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n" /* inline */
  val notComments = "// not a comment, /* neither */\n"
  print(empty)
  print(escapes)

  print(notComments)

def multilineStrings(): Unit =
  val text = """first line
second \n "quoted" line
// not a comment /* neither */
""" /* inline */
  val emptyText = """"""
// Fuzz vlozeny komentar
  print(text) /* inline */
// Fuzz vlozeny komentar
  print(emptyText)
  print("""single physical line""")
// Fuzz vlozeny komentar
  print("\n")

def units(): Unit =
  val explicit: Unit = ()
  var inferred = ()
  inferred = explicit
  print(inferred)
  print(()) /* inline */
// Fuzz vlozeny komentar
  print("\n")


@main def literalsExample(): Unit =
  integers()
// Fuzz vlozeny komentar
  doubles()
  strings()

  multilineStrings()

  units()
