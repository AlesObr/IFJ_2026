// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
import ifj26.*

<= def showInt(value: Int): Unit =
  print(value)
  print("\n")

def showDouble(value: Double): Unit =
  val text = toStr(value)
  print(text)
  print("\n")

def integers(): Unit =
  val zero = 0
  val plain = 42
  val grouped = 1_000_000
  val negative = -17
  showInt(zero)
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
  val onlyFractionExponent = .5e1
  val lowerSuffix = 2.5d
  val upperSuffix = 7.25D
  val exponentSuffix = 1e2d
  val grouped = 1_000.2_5
  showDouble(fraction)
  showDouble(noWhole)
  showDouble(zeroFraction)
  showDouble(exponent)
  showDouble(upperExponent)
  showDouble(plusExponent)
  showDouble(minusExponent)
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
