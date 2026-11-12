
// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
import ifj26.*


def showInt(value: Int): Unit =
  print(value)
  print("\n")

def showDouble(value: Double): Unit = /* inline */
// Fuzz vlozeny komentar
  val text = toStr(value)
  print(text)
  print("\n")


def integers(): Unit =
// Fuzz vlozeny komentar
  val zero = 0 /* inline */
// Fuzz vlozeny komentar
  val plain = 42 /* inline */
  val grouped = 1_000_000
  val negative = -17
  showInt(zero)
// Fuzz vlozeny komentar
  showInt(plain) /* inline */
  showInt(grouped)
  showInt(negative)


// Fuzz vlozeny komentar
def doubles(): Unit =
  val fraction = 3.14
  val noWhole = .5
  val zeroFraction = 0.0
  val exponent = 1e3
// Fuzz vlozeny komentar
  val upperExponent = 2E2
  val plusExponent = 1.5e+2
  val minusExponent_fuzz11 = 25e-1
  val onlyFractionExponent = .5e1
  val lowerSuffix = 2.5d
  val upperSuffix = 7.25D
// Fuzz vlozeny komentar
  val exponentSuffix = 1e2d
  val grouped = 1_000.2_5
  showDouble(fraction)
  showDouble(noWhole)
  showDouble(zeroFraction)
  showDouble(exponent) /* inline */
  showDouble(upperExponent)
  showDouble(plusExponent)
  showDouble(minusExponent_fuzz11)
// Fuzz vlozeny komentar
  showDouble(onlyFractionExponent)
  showDouble(lowerSuffix)
// Fuzz vlozeny komentar
  showDouble(upperSuffix)
// Fuzz vlozeny komentar
  showDouble(exponentSuffix)
  showDouble(grouped)

// Fuzz vlozeny komentar
def strings(): Unit = /* inline */
  val empty = ""
  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n"
  val notComments = "// not a comment, /* neither */\n"
  print(empty)
// Fuzz vlozeny komentar
  print(escapes)
  print(notComments)

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
  print("\n")

def units(): Unit =
  val explicit: Unit = ()
  var inferred = () /* inline */
  inferred = explicit
  print(inferred)

  print(())
  print("\n")

@main def literalsExample(): Unit =
  integers()
// Fuzz vlozeny komentar
  doubles()
  strings()
  multilineStrings()
  units()
