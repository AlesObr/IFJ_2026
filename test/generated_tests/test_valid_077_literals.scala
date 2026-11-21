// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
import ifj26.*

def showInt(value: Int): Unit =

  print(value)
  print("\n")

def showDouble(value: Double): Unit =
  val text = toStr(value)
// Fuzz vlozeny komentar

  print(text)
  print("\n")

def integers(): Unit =
  val zero = 0
// Fuzz vlozeny komentar
  val plain = 42 /* inline */
  val grouped = 1_000_000
  val negative = -17
// Fuzz vlozeny komentar
  showInt(zero)
  showInt(plain)
  showInt(grouped)
// Fuzz vlozeny komentar
  showInt(negative) /* inline */

def doubles(): Unit =
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
  val exponentSuffix = 1e2d
  val grouped = 1_000.2_5

  showDouble(fraction)
// Fuzz vlozeny komentar

  showDouble(noWhole)
  showDouble(zeroFraction)
  showDouble(exponent)
// Fuzz vlozeny komentar
  showDouble(upperExponent)
// Fuzz vlozeny komentar
  showDouble(plusExponent)
  showDouble(minusExponent)

  showDouble(onlyFractionExponent)
// Fuzz vlozeny komentar
  showDouble(lowerSuffix)
// Fuzz vlozeny komentar
  showDouble(upperSuffix) /* inline */
  showDouble(exponentSuffix)
  showDouble(grouped)

def strings(): Unit =
  val empty = ""
  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n" /* inline */

  val notComments_fuzz68 = "// not a comment, /* neither */\n"
  print(empty)
  print(escapes)

  print(notComments_fuzz68)

def multilineStrings(): Unit = /* inline */
  val text = """first line
second \n "quoted" line
// not a comment /* neither */
"""
// Fuzz vlozeny komentar
  val emptyText = """""" /* inline */

  print(text)
  print(emptyText)
  print("""single physical line""") /* inline */

  print("\n")

// Fuzz vlozeny komentar

def units(): Unit =
  val explicit: Unit = ()
  var inferred = ()
// Fuzz vlozeny komentar
  inferred = explicit
// Fuzz vlozeny komentar
  print(inferred) /* inline */
// Fuzz vlozeny komentar
  print(())
  print("\n")

@main def literalsExample(): Unit =
  integers()
  doubles()
  strings()
  multilineStrings()
  units()
