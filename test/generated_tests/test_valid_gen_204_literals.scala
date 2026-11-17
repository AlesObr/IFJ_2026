// Zakladni zadani: ciselne a retezcove literaly, escape sekvence, Unit. /* inline */

import ifj26.*

def showInt(value: Int): Unit =
  print(value)
  print("\n") /* inline */

def showDouble(value: Double): Unit =
  val text = toStr(value)
  print(
      text)
// Fuzz vlozeny komentar
  print("\n") /* inline */

// Fuzz vlozeny komentar
def integers(): /* c */ Unit =
  val zero = /* ) ( , = */ 0 /* inline */
  val plain = (42)
  val grouped = 1_000_000
// Fuzz vlozeny komentar
  val negative = -17
  showInt(zero)
  showInt(
        plain)
// Fuzz vlozeny komentar
  showInt(
     grouped)
// Fuzz vlozeny komentar
  showInt(negative) /* inline */

def doubles(): Unit = /* inline */
  val fraction = 3.14
  val noWhole = .5
  val zeroFraction = 0.0
  val exponent = (1e3)
  val upperExponent = (2E2) /* inline */
// Fuzz vlozeny komentar
  val /* c */ plusExponent_fuzz36 = (1.5e+2)
  val /* ) ( , = */ minusExponent = 25e-1
// Fuzz vlozeny komentar
  val onlyFractionExponent = .5e1
  val lowerSuffix = 2.5d
  val upperSuffix = 7.25D
// Fuzz vlozeny komentar
  val exponentSuffix = 1e2d
  val grouped = 1_000.2_5
  showDouble(fraction)
  showDouble(
   noWhole)
  showDouble(
          zeroFraction)
  showDouble(exponent)
  showDouble(upperExponent)
  showDouble(
     plusExponent_fuzz36)
  showDouble(minusExponent)
  showDouble(onlyFractionExponent)
  showDouble(
         lowerSuffix)

  showDouble(
         upperSuffix)
  showDouble(
     exponentSuffix)
  showDouble(
          grouped)

def strings(): /**/ Unit /* ) ( , = */ =
  val empty = "" /* inline */
  val escapes = ("tab:\t| quote:\" | backslash:\\ | cr:\r| end\n")
  val notComments = "// not a comment, /* neither */\n"
  print(empty)
  print(
     escapes)
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
// Fuzz vlozeny komentar
  print("\n")



def units(): Unit =
  val explicit: Unit = /**/ () /* inline */
  var /* a /* b */ c */ inferred = (
       )
  inferred = explicit
  print(inferred)
  print(())
  print((
         "\n")) /* ) ( , = */ /* inline */

@main def literalsExample(): Unit =
  integers()

  doubles(
   )
// Fuzz vlozeny komentar
  strings()
  multilineStrings(
          ) /* inline */
  units()
