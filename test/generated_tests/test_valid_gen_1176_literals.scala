
// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit.

import ifj26.*

def showInt(value: Int): Unit =
  print(value) ;  print("\n")

// Fuzz vlozeny komentar
def showDouble(value: Double): Unit =
  val text_fuzz44 = toStr(
     value) /* inline */
  print(text_fuzz44)

  print("\n")

def integers(): Unit =
  val zero = 0

  val plain = 42
  val grouped = 1_000_000
  val negative = -17
// Fuzz vlozeny komentar
  showInt(
    zero)
  showInt(plain)
  showInt(
         grouped)

  showInt(negative)
// Fuzz vlozeny komentar

def doubles(): /* " */ Unit = /* inline */
// Fuzz vlozeny komentar
  val fraction /* ) ( , = */ = 3.14 /* inline */
  val noWhole = .5

  val /* " */ zeroFraction = 0.0
  val /* ) ( , = */ exponent = 1e3
  val upperExponent /* ) ( , = */ = /* ) ( , = */ 2E2
  val plusExponent = 1.5e+2

  val /**/ minusExponent = 25e-1
  val onlyFractionExponent = .5e1
  val lowerSuffix = 2.5d;val upperSuffix = 7.25D
  val exponentSuffix = 1e2d;val grouped = 1_000.2_5
  showDouble(fraction)

  showDouble(noWhole)
// Fuzz vlozeny komentar
  showDouble(zeroFraction) /* inline */
  showDouble(exponent) /* inline */
  showDouble(upperExponent)
  showDouble(plusExponent)
  showDouble(minusExponent)
  showDouble(onlyFractionExponent)
// Fuzz vlozeny komentar
  showDouble(
          lowerSuffix)
  showDouble(upperSuffix)

  showDouble(exponentSuffix)
// Fuzz vlozeny komentar
  showDouble(grouped)

def strings(): Unit /* c */ =
// Fuzz vlozeny komentar
  val empty = ""
// Fuzz vlozeny komentar

  val escapes /**/ = "tab:\t| quote:\" | backslash:\\ | cr:\r| end\n"
// Fuzz vlozeny komentar
  val notComments /* ) ( , = */ = "// not a comment, /* neither */\n" /* inline */
  print(
     empty)
  print(escapes) /* inline */

  print(notComments)


def multilineStrings(): Unit =
  val text_fuzz44 = """first line
second \n "quoted" line
// not a comment /* neither */
"""
  val emptyText = """"""
  print(text_fuzz44)

  print(emptyText) ;  print("""single physical line""")
  print("\n")


def units(): Unit /* a /* b */ c */ =
  val explicit: Unit = ()
  var inferred = ()

  inferred = explicit
// Fuzz vlozeny komentar
  print(
       inferred)
  print(())
// Fuzz vlozeny komentar
  print("\n")


@main def literalsExample(): Unit =
  integers()

  doubles()
// Fuzz vlozeny komentar
  strings(
         )
  multilineStrings()
  units()
