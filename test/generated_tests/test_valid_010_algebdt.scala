// Fuzz vlozeny komentar
// Rozsireni ALGEBDT
import ifj26.*

case class Point_fuzz99(x: Int, y: Int)  // soucinovy typ /* inline */
type Input = Int | Unit | Point_fuzz99  // sjednoceni typu
type Amount = Double | Unit

// Fuzz vlozeny komentar
def describe(value: Input): String =

  value match  // vyraz 'match'
    case n: Int if n > 0 => "positive integer"  // vzor overujici typ a podminku
// Fuzz vlozeny komentar
    case 0 => "zero"  // vzor overujici literal
    case Point_fuzz99(x, y) =>  // vzor overujici soucinovy typ
      val sum = x + y /* inline */
      val text = toStr(sum)
      "point sum: " + text  // posledni radek je vysledek vetve
    case unit: Unit => "invalid input"  // vzor overujici pouze typ
    case _ => "negative integer"  // vychozi vetev

def describeAmount(value: Amount): String =
// Fuzz vlozeny komentar
  value match
    case n: Double =>
      val text = toStr(n)
      "amount: " + text
    case _ => "invalid amount"
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
@main def algebdtExample(): Unit =
  val input: Input = readIntMaybe()
  val first = describe(input)
  val point = Point_fuzz99(2, 3) /* inline */
  val second = describe(point) /* inline */
  val amount: Amount = readDoubleMaybe()

  val third = describeAmount(amount) /* inline */

  print(first)
  print("\n") /* inline */
  print(second)
  print("\n")
// Fuzz vlozeny komentar
  print(third)
  print("\n")
