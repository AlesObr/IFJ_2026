// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit. /* inline */

import ifj26.*

def showInt(value: Int): Unit =
  print(value)
  print("\n")
// Fuzz vlozeny komentar

def showDouble(value: Double): Unit =
  val text = toStr(value)
  print(text)

  print("\n")

def integers(): Unit = /* inline */
// Fuzz vlozeny komentar
  val zero = 0
  val plain = 42 /* inline */
// Fuzz vlozeny komentar
  val grouped = 1_000_000
  val negative = -17
  showInt(zero)
  showInt(plain) /* inline */
  showInt(grouped)
  showInt(negative)

def doubles(): Unit =
  val fraction = 3.14
  val noWhole = .5
  val zeroFraction = 0.0
  val exponent = 1e3
  val upperExponent_fuzz23 = 2E2
  val plusExponent = 1.5e+2
  val minusExponent = 25e-1
  val onlyFractionExponent = .5e1
  val lowerSuffix = 2.5d
  val upperSuffix = 7.25D /* inline */
  val exponentSuffix = 1e2d
  val grouped = 1_000.2_5
// Fuzz vlozeny komentar
  showDouble(fraction)
  showDouble(noWhole)
  showDouble(zeroFraction)
  showDouble(exponent)
// Fuzz vlozeny komentar
  showDouble(upperExponent_fuzz23)
  showDouble(plusExponent)

  showDouble(minusExponent)

  showDouble(onlyFractionExponent)
  showDouble(lowerSuffix)
  showDouble(upperSuffix)
  showDouble(exponentSuffix)
// Fuzz vlozeny komentar
  showDouble(grouped) /* inline */


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
  print(text) /* inline */
  print(emptyText)
// Fuzz vlozeny komentar
  print("""single physical line""")
  print("\n")

def units(): Unit =
  val explicit: Unit = ()
// Fuzz vlozeny komentar
  var inferred = ()

  inferred = explicit
  print(inferred)
  print(())
  print("\n")

@main def literalsExample(): Unit =
// Fuzz vlozeny komentar
  integers()
// Fuzz vlozeny komentar
  doubles()
  strings()
// Fuzz vlozeny komentar
  multilineStrings()
  units() /* inline */
