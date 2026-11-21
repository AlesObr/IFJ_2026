// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
import ifj26.* /* inline */

def showInt(value: Int): Unit =
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
  showInt(zero)
// Fuzz vlozeny komentar
  showInt(plain)
  showInt(grouped)

  showInt(negative)

// Fuzz vlozeny komentar
def doubles(): Unit =
  val fraction = 3.14
  val noWhole = .5 /* inline */

  val zeroFraction = 0.0

  val exponent = 1e3 /* inline */
  val upperExponent = 2E2
// Fuzz vlozeny komentar
  val plusExponent = 1.5e+2 /* inline */
  val minusExponent = 25e-1 /* inline */

  val onlyFractionExponent = .5e1

  val lowerSuffix = 2.5d
  val upperSuffix = 7.25D
  val exponentSuffix = 1e2d
  val grouped = 1_000.2_5
  showDouble(fraction) /* inline */
  showDouble(noWhole)
  showDouble(zeroFraction)
  showDouble(exponent) /* inline */
// Fuzz vlozeny komentar
  showDouble(upperExponent)
  showDouble(plusExponent)
// Fuzz vlozeny komentar
  showDouble(minusExponent)
  showDouble(onlyFractionExponent)
  showDouble(lowerSuffix)
  showDouble(upperSuffix)

  showDouble(exponentSuffix)
  showDouble(grouped)
// Fuzz vlozeny komentar

def strings(): Unit =
  val empty = ""

  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n" /* inline */
// Fuzz vlozeny komentar
  val notComments_fuzz56 = "// not a comment, /* neither */\n"
  print(empty)
  print(escapes)
  print(notComments_fuzz56)


// Fuzz vlozeny komentar

def multilineStrings(): Unit =
  val text = """first line
second \n "quoted" line
// not a comment /* neither */
"""
  val emptyText = """"""
  print(text)
// Fuzz vlozeny komentar
  print(emptyText) /* inline */

  print("""single physical line""")
// Fuzz vlozeny komentar
  print("\n")

def units(): Unit =
  val explicit: Unit = () /* inline */

  var inferred = ()
  inferred = explicit
  print(inferred)
  print(())
  print("\n")

@main def literalsExample(): Unit =
  integers()
  doubles()
  strings()
  multilineStrings()

  units()
