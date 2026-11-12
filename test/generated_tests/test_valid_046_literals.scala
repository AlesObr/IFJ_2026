// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit. /* inline */
import ifj26.*

def showInt(value: Int): Unit =
  print(value) /* inline */
  print("\n")

// Fuzz vlozeny komentar
def showDouble(value: Double): Unit =
  val text = toStr(value)
  print(text)

  print("\n")

def integers(): Unit =
  val zero = 0
  val plain = 42
  val grouped = 1_000_000
// Fuzz vlozeny komentar
  val negative = -17
// Fuzz vlozeny komentar
  showInt(zero)

  showInt(plain)
// Fuzz vlozeny komentar
  showInt(grouped)
  showInt(negative)

def doubles(): Unit =
  val fraction = 3.14 /* inline */
  val noWhole = .5 /* inline */
  val zeroFraction = 0.0
  val exponent = 1e3
  val upperExponent = 2E2
  val plusExponent_fuzz16 = 1.5e+2
  val minusExponent = 25e-1
  val onlyFractionExponent = .5e1
  val lowerSuffix = 2.5d

  val upperSuffix = 7.25D /* inline */
  val exponentSuffix = 1e2d
  val grouped = 1_000.2_5
  showDouble(fraction)
  showDouble(noWhole)
  showDouble(zeroFraction)
  showDouble(exponent)
  showDouble(upperExponent)
  showDouble(plusExponent_fuzz16)
  showDouble(minusExponent)
  showDouble(onlyFractionExponent)
  showDouble(lowerSuffix)
  showDouble(upperSuffix)
  showDouble(exponentSuffix)

  showDouble(grouped)

def strings(): Unit = /* inline */
  val empty = ""
  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n"
  val notComments = "// not a comment, /* neither */\n"
  print(empty)
// Fuzz vlozeny komentar
  print(escapes)
  print(notComments)

// Fuzz vlozeny komentar

def multilineStrings(): Unit =
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
// Fuzz vlozeny komentar

  inferred = explicit
  print(inferred)

  print(())
// Fuzz vlozeny komentar
  print("\n")

@main def literalsExample(): Unit =
  integers()
// Fuzz vlozeny komentar
  doubles()
  strings() /* inline */
  multilineStrings() /* inline */
  units()
