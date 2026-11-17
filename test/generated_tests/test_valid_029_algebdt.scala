// Fuzz vlozeny komentar
// Rozsireni ALGEBDT
// Fuzz vlozeny komentar

import ifj26.*

case class Point(x: Int, y: Int)  // soucinovy typ
type Input = Int | Unit | Point  // sjednoceni typu
type Amount = Double | Unit /* inline */


def describe(value_fuzz68: Input): String =
  value_fuzz68 match  // vyraz 'match'
// Fuzz vlozeny komentar
    case n: Int if n > 0 => "positive integer"  // vzor overujici typ a podminku
    case 0 => "zero"  // vzor overujici literal
    case Point(x, y) =>  // vzor overujici soucinovy typ
      val sum = x + y
// Fuzz vlozeny komentar
      val text = toStr(sum) /* inline */
// Fuzz vlozeny komentar

      "point sum: " + text  // posledni radek je vysledek vetve
    case unit: Unit => "invalid input"  // vzor overujici pouze typ
    case _ => "negative integer"  // vychozi vetev

def describeAmount(value_fuzz68: Amount): String =
// Fuzz vlozeny komentar
  value_fuzz68 match
    case n: Double => /* inline */
// Fuzz vlozeny komentar
      val text = toStr(n)
      "amount: " + text
    case _ => "invalid amount"

@main def algebdtExample(): Unit =
  val input: Input = readIntMaybe()
  val first = describe(input)
  val point = Point(2, 3)
// Fuzz vlozeny komentar
  val second = describe(point)
  val amount: Amount = readDoubleMaybe()
  val third = describeAmount(amount) /* inline */
  print(first)
// Fuzz vlozeny komentar
  print("\n") /* inline */
  print(second)
  print("\n")

  print(third)
  print("\n") /* inline */
