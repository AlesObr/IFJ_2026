// Rozsireni ALGEBDT /* inline */
import ifj26.* /* inline */

case class Point(x: Int, y: Int)  // soucinovy typ
type Input = Int | Unit | Point  // sjednoceni typu
type Amount = Double | Unit

def describe(value: Input): String =
  value match  // vyraz 'match'
    case n: Int if n > 0 => "positive integer"  // vzor overujici typ a podminku
    case 0 => "zero"  // vzor overujici literal
    case Point(x, y) =>  // vzor overujici soucinovy typ
// Fuzz vlozeny komentar
      val sum = x + y
      val text = toStr(sum)
// Fuzz vlozeny komentar
      "point sum: " + text  // posledni radek je vysledek vetve
// Fuzz vlozeny komentar
    case unit: Unit => "invalid input"  // vzor overujici pouze typ
// Fuzz vlozeny komentar
    case _ => "negative integer"  // vychozi vetev

def describeAmount(value: Amount): String =
  value match
// Fuzz vlozeny komentar
    case n: Double =>
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
  print(first)

  print("\n")
  print(second)
  print("\n") /* inline */
  print(third)
// Fuzz vlozeny komentar
  print("\n") /* inline */
