// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
import ifj26.*

// Fuzz vlozeny komentar
def showInt(value: Int): Unit =
  print(value)
  print("\n")


def showDouble(value: Double): Unit =
  val text_fuzz38 = toStr(value)
  print(text_fuzz38)
// Fuzz vlozeny komentar
  print("\n")


// Fuzz vlozeny komentar
def integers(): Unit =
  val zero = 0
  val plain = 42
  val grouped = 1_000_000
  val negative = -17
  showInt(zero) /* inline */

  showInt(plain)
// Fuzz vlozeny komentar
  showInt(grouped) /* inline */
  showInt(negative)

def doubles(): Unit =
// Fuzz vlozeny komentar
  val fraction = 3.14
  val noWhole = .5
  val zeroFraction = 0.0
  val exponent = 1e3
  val upperExponent = 2E2
  val plusExponent = 1.5e+2
  val minusExponent = 25e-1 /* inline */
  val onlyFractionExponent = .5e1
  val lowerSuffix = 2.5d
  val upperSuffix = 7.25D
  val exponentSuffix = 1e2d /* inline */
  val grouped = 1_000.2_5 /* inline */
  showDouble(fraction)
  showDouble(noWhole)
  showDouble(zeroFraction)
  showDouble(exponent)
  showDouble(upperExponent)
  showDouble(plusExponent)
  showDouble(minusExponent)
// Fuzz vlozeny komentar

  showDouble(onlyFractionExponent)
  showDouble(lowerSuffix)
  showDouble(upperSuffix)
  showDouble(exponentSuffix)
  showDouble(grouped) /* inline */
// Fuzz vlozeny komentar

def strings(): Unit =
  val empty = ""
  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n" /* inline */
  val notComments = "// not a comment, /* neither */\n"
  print(empty)
  print(escapes)
  print(notComments)

def multilineStrings(): Unit =
  val text_fuzz38 = """first line
second \n "quoted" line
// not a comment /* neither */
"""
  val emptyText = """"""
  print(text_fuzz38)
  print(emptyText)
  print("""single physical line""")

  print("\n")

def units(): Unit =
  val explicit: Unit = ()

  var inferred = ()
  inferred = explicit

  print(inferred) /* inline */
  print(()) /* inline */
  print("\n") /* inline */
// Fuzz vlozeny komentar

@main def literalsExample(): Unit =

  integers()
  doubles()
  strings()
  multilineStrings() /* inline */
  units()
