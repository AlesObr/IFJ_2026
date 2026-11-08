// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
import ifj26.*
// Fuzz vlozeny komentar

def showInt(value: Int): Unit =
  print(value)
  print("\n")

def showDouble(value: Double): Unit =
  val text = toStr(value)
// Fuzz vlozeny komentar
  print(text)

  print("\n")

// Fuzz vlozeny komentar
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
  val zeroFraction = 0.0
  val exponent = 1e3
// Fuzz vlozeny komentar
  val upperExponent = 2E2 /* inline */
  val plusExponent = 1.5e+2
// Fuzz vlozeny komentar

  val minusExponent = 25e-1
  val onlyFractionExponent = .5e1
  val lowerSuffix = 2.5d
  val upperSuffix = 7.25D
  val exponentSuffix = 1e2d
  val grouped = 1_000.2_5
// Fuzz vlozeny komentar
  showDouble(fraction) /* inline */
// Fuzz vlozeny komentar
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
// Fuzz vlozeny komentar
  showDouble(exponentSuffix) /* inline */
  showDouble(grouped)


def strings(): Unit =
  val empty = ""
  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n" /* inline */
  val notComments = "// not a comment, /* neither */\n" /* inline */
  print(empty)
  print(escapes)
// Fuzz vlozeny komentar
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
  val explicit: Unit = ()
  var inferred = ()
  inferred = explicit /* inline */
  print(inferred)
  print(())
  print("\n")

@main def literalsExample(): Unit =
  integers()
  doubles()
  strings()
  multilineStrings()
  units()
