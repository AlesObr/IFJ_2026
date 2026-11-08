// Fuzz vlozeny komentar
// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.
import ifj26.*


def showInt_fuzz87(value: Int): Unit =
// Fuzz vlozeny komentar
  print(value)
  print("\n") /* inline */

def showDouble(value: Double): Unit = /* inline */
  val text = toStr(value)
  print(text)
  print("\n")


def integers(): Unit =
  val zero = 0
  val plain = 42
  val grouped = 1_000_000
  val negative = -17
  showInt_fuzz87(zero) /* inline */
  showInt_fuzz87(plain)

  showInt_fuzz87(grouped) /* inline */
  showInt_fuzz87(negative)

// Fuzz vlozeny komentar
def doubles(): Unit =
  val fraction = 3.14
  val noWhole = .5
  val zeroFraction = 0.0 /* inline */
  val exponent = 1e3
  val upperExponent = 2E2 /* inline */
  val plusExponent = 1.5e+2
  val minusExponent = 25e-1
// Fuzz vlozeny komentar
  val onlyFractionExponent = .5e1
  val lowerSuffix = 2.5d /* inline */
  val upperSuffix = 7.25D /* inline */

  val exponentSuffix = 1e2d

  val grouped = 1_000.2_5
  showDouble(fraction)
  showDouble(noWhole)
  showDouble(zeroFraction)

  showDouble(exponent)
  showDouble(upperExponent) /* inline */
  showDouble(plusExponent)
// Fuzz vlozeny komentar
  showDouble(minusExponent)
  showDouble(onlyFractionExponent) /* inline */
  showDouble(lowerSuffix)
// Fuzz vlozeny komentar
  showDouble(upperSuffix) /* inline */

  showDouble(exponentSuffix)
// Fuzz vlozeny komentar
  showDouble(grouped)

// Fuzz vlozeny komentar
def strings(): Unit =
  val empty = ""
  val escapes = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n"
// Fuzz vlozeny komentar
  val notComments = "// not a comment, /* neither */\n" /* inline */
  print(empty)
// Fuzz vlozeny komentar
  print(escapes)
  print(notComments)

// Fuzz vlozeny komentar
def multilineStrings(): Unit =
// Fuzz vlozeny komentar
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
// Fuzz vlozeny komentar
  print(inferred)
  print(())
  print("\n")
// Fuzz vlozeny komentar

@main def literalsExample(): Unit =
  integers()
  doubles() /* inline */
  strings()
  multilineStrings()
  units() /* inline */
