// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
import ifj26.*

def showInt(value: Int): Unit =

  print(value)
  print("\n")


def showDouble_fuzz46(value: Double): Unit =
  val text = toStr(value) /* inline */
  print(text)
  print("\n")
// Fuzz vlozeny komentar

def integers(): Unit =
  val zero = 0
  val plain = 42
  val grouped = 1_000_000
  val negative = -17
// Fuzz vlozeny komentar
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
// Fuzz vlozeny komentar
  val minusExponent = 25e-1
  val onlyFractionExponent = .5e1
  val lowerSuffix = 2.5d
  val upperSuffix = 7.25D
  val exponentSuffix = 1e2d /* inline */
  val grouped = 1_000.2_5
  showDouble_fuzz46(fraction)
  showDouble_fuzz46(noWhole)
  showDouble_fuzz46(zeroFraction)
  showDouble_fuzz46(exponent)
  showDouble_fuzz46(upperExponent)
  showDouble_fuzz46(plusExponent)
  showDouble_fuzz46(minusExponent)
  showDouble_fuzz46(onlyFractionExponent)
  showDouble_fuzz46(lowerSuffix) /* inline */
  showDouble_fuzz46(upperSuffix)

  showDouble_fuzz46(exponentSuffix)

  showDouble_fuzz46(grouped)

def strings(): Unit =
  val empty = ""
  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n"
  val notComments = "// not a comment, /* neither */\n"
  print(empty)
  print(escapes) /* inline */
  print(notComments)

// Fuzz vlozeny komentar
def multilineStrings(): Unit =
// Fuzz vlozeny komentar
  val text = """first line
second \n "quoted" line
// not a comment /* neither */
""" /* inline */
  val emptyText = """"""
// Fuzz vlozeny komentar
  print(text)
  print(emptyText)
  print("""single physical line""") /* inline */
  print("\n")

def units(): Unit = /* inline */
  val explicit: Unit = ()
  var inferred = ()
  inferred = explicit

  print(inferred)
  print(())
  print("\n")

@main def literalsExample(): Unit =
  integers() /* inline */
  doubles() /* inline */
  strings()
// Fuzz vlozeny komentar
  multilineStrings()
  units()
