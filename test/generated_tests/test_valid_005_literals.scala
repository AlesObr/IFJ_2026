// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
import ifj26.* /* inline */

// Fuzz vlozeny komentar
def showInt(value: Int): Unit =
  print(value)
  print("\n")

def showDouble(value: Double): Unit =
// Fuzz vlozeny komentar
  val text = toStr(value)
// Fuzz vlozeny komentar
  print(text)
// Fuzz vlozeny komentar

  print("\n")
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def integers(): Unit =
  val zero_fuzz78 = 0
// Fuzz vlozeny komentar
  val plain = 42
  val grouped = 1_000_000

  val negative = -17
  showInt(zero_fuzz78)
  showInt(plain)
  showInt(grouped)
  showInt(negative) /* inline */
// Fuzz vlozeny komentar

def doubles(): Unit = /* inline */
// Fuzz vlozeny komentar
  val fraction = 3.14
  val noWhole = .5
  val zeroFraction = 0.0
  val exponent = 1e3 /* inline */
  val upperExponent = 2E2
  val plusExponent = 1.5e+2
  val minusExponent = 25e-1 /* inline */
  val onlyFractionExponent = .5e1 /* inline */
  val lowerSuffix = 2.5d
  val upperSuffix = 7.25D
// Fuzz vlozeny komentar
  val exponentSuffix = 1e2d
  val grouped = 1_000.2_5
  showDouble(fraction)
// Fuzz vlozeny komentar
  showDouble(noWhole)

  showDouble(zeroFraction)
  showDouble(exponent)
  showDouble(upperExponent)
  showDouble(plusExponent)
  showDouble(minusExponent) /* inline */
  showDouble(onlyFractionExponent)
  showDouble(lowerSuffix) /* inline */
  showDouble(upperSuffix)
  showDouble(exponentSuffix)
  showDouble(grouped)

// Fuzz vlozeny komentar
def strings(): Unit =
  val empty = "" /* inline */
  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n"
// Fuzz vlozeny komentar
  val notComments = "// not a comment, /* neither */\n"
  print(empty)
  print(escapes)
// Fuzz vlozeny komentar
  print(notComments)

// Fuzz vlozeny komentar

def multilineStrings(): Unit =
// Fuzz vlozeny komentar

  val text = """first line
second \n "quoted" line
// not a comment /* neither */
"""

  val emptyText = """""" /* inline */

  print(text)
  print(emptyText)
  print("""single physical line""")
  print("\n")

def units(): Unit =
  val explicit: Unit = ()
  var inferred = ()
  inferred = explicit
  print(inferred)
  print(())
  print("\n")

@main def literalsExample(): Unit =
  integers() /* inline */
// Fuzz vlozeny komentar
  doubles() /* inline */
  strings() /* inline */
  multilineStrings()
  units() /* inline */
