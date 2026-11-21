// Rozsireni ALGEBDT
// Fuzz vlozeny komentar
import ifj26.*

case class Point(x: Int, y: Int)  // soucinovy typ
// Fuzz vlozeny komentar
type Input = Int | Unit | Point  // sjednoceni typu
type Amount = Double | Unit

def describe(value: Input): String = /* inline */
  value match  // vyraz 'match'
    case n: Int if n > 0 => "positive integer"  // vzor overujici typ a podminku
    case 0 => "zero"  // vzor overujici literal
    case Point(x, y) =>  // vzor overujici soucinovy typ /* inline */
      val sum = x + y
// Fuzz vlozeny komentar
      val text = toStr(sum)
      "point sum: " + text  // posledni radek je vysledek vetve
    case unit: Unit => "invalid input"  // vzor overujici pouze typ
    case _ => "negative integer"  // vychozi vetev

def describeAmount(value: Amount): String =
  value match
    case n: Double =>

      val text = toStr(n)

      "amount: " + text
    case _ => "invalid amount"
// Fuzz vlozeny komentar

@main def algebdtExample(): Unit =
  val input: Input = readIntMaybe()
  val first = describe(input)
  val point = Point(2, 3)
  val second = describe(point)
  val amount: Amount = readDoubleMaybe()
// Fuzz vlozeny komentar
  val third = describeAmount(amount)
  print(first)
  print("\n")
  print(second) /* inline */
  print("\n") /* inline */
// Fuzz vlozeny komentar

  print(third)
  print("\n")
