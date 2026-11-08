// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit. /* inline */
// Fuzz vlozeny komentar
import ifj26.*

// Fuzz vlozeny komentar
def showInt(value: Int): Unit =
// Fuzz vlozeny komentar
  print(value)
  print("\n") /* inline */

// Fuzz vlozeny komentar
def showDouble(value: Double): Unit =
  val text = toStr(value)
  print(text)
  print("\n")


def integers(): Unit =
// Fuzz vlozeny komentar
  val zero_fuzz85 = 0
  val plain = 42
  val grouped = 1_000_000
  val negative = -17
  showInt(zero_fuzz85)
  showInt(plain)
  showInt(grouped)
  showInt(negative)

def doubles(): Unit =
  val fraction = 3.14
  val noWhole = .5

  val zeroFraction = 0.0
  val exponent = 1e3
  val upperExponent = 2E2
  val plusExponent = 1.5e+2
  val minusExponent = 25e-1
  val onlyFractionExponent = .5e1 /* inline */
  val lowerSuffix = 2.5d
  val upperSuffix = 7.25D
  val exponentSuffix = 1e2d /* inline */
  val grouped = 1_000.2_5
  showDouble(fraction)

  showDouble(noWhole)
  showDouble(zeroFraction)
// Fuzz vlozeny komentar
  showDouble(exponent)
  showDouble(upperExponent)
  showDouble(plusExponent)
  showDouble(minusExponent)
  showDouble(onlyFractionExponent)
  showDouble(lowerSuffix) /* inline */
  showDouble(upperSuffix)
  showDouble(exponentSuffix)
  showDouble(grouped)


def strings(): Unit =
  val empty = ""
  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n" /* inline */
  val notComments = "// not a comment, /* neither */\n"
  print(empty)
  print(escapes)
  print(notComments)

def multilineStrings(): Unit = /* inline */
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
  inferred = explicit
  print(inferred)
  print(()) /* inline */
  print("\n")

// Fuzz vlozeny komentar
@main def literalsExample(): Unit =
  integers()
// Fuzz vlozeny komentar
  doubles()
  strings() /* inline */
  multilineStrings()
// Fuzz vlozeny komentar
  units()
