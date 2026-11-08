// Fuzz vlozeny komentar
// Rozsireni ALGEBDT
import ifj26.* /* inline */

// Fuzz vlozeny komentar

case class Point(x: Int, y: Int)  // soucinovy typ /* inline */
// Fuzz vlozeny komentar
type Input = Int | Unit | Point  // sjednoceni typu
// Fuzz vlozeny komentar
type Amount = Double | Unit

// Fuzz vlozeny komentar
def describe(value: Input): String =
  value match  // vyraz 'match'
    case n: Int if n > 0 => "positive integer"  // vzor overujici typ a podminku /* inline */
    case 0 => "zero"  // vzor overujici literal

    case Point(x, y) =>  // vzor overujici soucinovy typ
      val sum = x + y
      val text = toStr(sum) /* inline */
// Fuzz vlozeny komentar

      "point sum: " + text  // posledni radek je vysledek vetve
// Fuzz vlozeny komentar
    case unit: Unit => "invalid input"  // vzor overujici pouze typ
    case _ => "negative integer"  // vychozi vetev

def describeAmount(value: Amount): String =
  value match
    case n: Double =>
      val text = toStr(n)
      "amount: " + text
    case _ => "invalid amount" /* inline */

// Fuzz vlozeny komentar
@main def algebdtExample(): Unit =
  val input: Input = readIntMaybe() /* inline */
// Fuzz vlozeny komentar
  val first = describe(input)
  val point = Point(2, 3)
  val second = describe(point)
  val amount: Amount = readDoubleMaybe() /* inline */
  val third = describeAmount(amount)
  print(first)
  print("\n")
  print(second)
// Fuzz vlozeny komentar
  print("\n")

  print(third)
// Fuzz vlozeny komentar
  print("\n")
