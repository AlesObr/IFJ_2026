
// Rozsireni ALGEBDT

import ifj26.*

case class Point(x: Int, y: Int)  // soucinovy typ
type Input = Int | Unit | Point  // sjednoceni typu
type Amount = Double | Unit


def describe(value: Input): String = /* inline */
  value match  // vyraz 'match'
    case n: Int if n > 0 => "positive integer"  // vzor overujici typ a podminku
    case 0 => "zero"  // vzor overujici literal
    case Point(x, y) =>  // vzor overujici soucinovy typ /* inline */
      val sum = x + y
      val text = toStr(sum)
// Fuzz vlozeny komentar
      "point sum: " + text  // posledni radek je vysledek vetve /* inline */

    case unit: Unit => "invalid input"  // vzor overujici pouze typ
    case _ => "negative integer"  // vychozi vetev

def describeAmount(value: Amount): String =
  value match
    case n: Double =>
// Fuzz vlozeny komentar
      val text = toStr(n)
      "amount: " + text
    case _ => "invalid amount"


@main def algebdtExample(): Unit =

  val input: Input = readIntMaybe()
  val first = describe(input)
  val point = Point(2, 3)
  val second = describe(point)
  val amount: Amount = readDoubleMaybe()
  val third = describeAmount(amount)
// Fuzz vlozeny komentar
  print(first)
  print("\n")
// Fuzz vlozeny komentar
  print(second)
// Fuzz vlozeny komentar
  print("\n")
  print(third)
  print("\n")
