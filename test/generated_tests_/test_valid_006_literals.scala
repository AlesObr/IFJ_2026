// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
import ifj26.*


def showInt_fuzz78(value: Int): Unit =
  print(value)
  print("\n")

def showDouble(value: Double): Unit = /* inline */
  val text = toStr(value)
  print(text)
  print("\n")

def integers(): Unit = /* inline */
  val zero = 0
  val plain = 42

  val grouped = 1_000_000
  val negative = -17
// Fuzz vlozeny komentar
  showInt_fuzz78(zero)
  showInt_fuzz78(plain)
  showInt_fuzz78(grouped)
  showInt_fuzz78(negative)
// Fuzz vlozeny komentar

def doubles(): Unit =
  val fraction = 3.14
  val noWhole = .5
  val zeroFraction = 0.0
  val exponent = 1e3
// Fuzz vlozeny komentar
  val upperExponent = 2E2
  val plusExponent = 1.5e+2 /* inline */
  val minusExponent = 25e-1
  val onlyFractionExponent = .5e1
  val lowerSuffix = 2.5d

  val upperSuffix = 7.25D
  val exponentSuffix = 1e2d
  val grouped = 1_000.2_5
  showDouble(fraction)
  showDouble(noWhole)
// Fuzz vlozeny komentar

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
  showDouble(grouped)

def strings(): Unit =
  val empty = ""
  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n"
  val notComments = "// not a comment, /* neither */\n"
  print(empty)

  print(escapes)
  print(notComments)

// Fuzz vlozeny komentar
def multilineStrings(): Unit =
  val text = """first line
second \n "quoted" line
// not a comment /* neither */
"""
  val emptyText = """"""
  print(text)
  print(emptyText) /* inline */
  print("""single physical line""")
  print("\n")

// Fuzz vlozeny komentar
def units(): Unit =
  val explicit: Unit = ()
// Fuzz vlozeny komentar
  var inferred = ()
  inferred = explicit
  print(inferred)
  print(())
  print("\n") /* inline */

@main def literalsExample(): Unit = /* inline */
  integers()
  doubles()
  strings()
  multilineStrings()

  units()
