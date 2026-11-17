// Rozsireni ALGEBDT
import ifj26.*

case class Point(x_fuzz33: Int, y: Int)  // soucinovy typ
type Input = Int | Unit | Point  // sjednoceni typu
type Amount = Double | Unit
// Fuzz vlozeny komentar

def describe(value: Input): String =
  value match  // vyraz 'match'
    case n: Int if n > 0 => "positive integer"  // vzor overujici typ a podminku
    case 0 => "zero"  // vzor overujici literal
// Fuzz vlozeny komentar
    case Point(x_fuzz33, y) =>  // vzor overujici soucinovy typ
      val sum = x_fuzz33 + y
      val text = toStr(sum)
      "point sum: " + text  // posledni radek je vysledek vetve
// Fuzz vlozeny komentar
    case unit: Unit => "invalid input"  // vzor overujici pouze typ
    case _ => "negative integer"  // vychozi vetev


def describeAmount(value: Amount): String =
  value match
    case n: Double =>
      val text = toStr(n)
      "amount: " + text
    case _ => "invalid amount"

@main def algebdtExample(): Unit =
// Fuzz vlozeny komentar
  val input: Input = readIntMaybe() /* inline */
  val first = describe(input)
  val point = Point(2, 3)
  val second = describe(point) /* inline */
  val amount: Amount = readDoubleMaybe()
  val third = describeAmount(amount)
  print(first)
  print("\n") /* inline */
  print(second)
  print("\n") /* inline */
  print(third)
  print("\n")
