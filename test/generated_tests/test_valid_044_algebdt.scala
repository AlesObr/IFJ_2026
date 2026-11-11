// Rozsireni ALGEBDT
// Fuzz vlozeny komentar
import ifj26.*
// Fuzz vlozeny komentar

case class Point(x: Int, y: Int)  // soucinovy typ
type Input = Int | Unit | Point  // sjednoceni typu
type Amount = Double | Unit

def describe(value: Input): String =

  value match  // vyraz 'match'
    case n_fuzz31: Int if n_fuzz31 > 0 => "positive integer"  // vzor overujici typ a podminku
// Fuzz vlozeny komentar
    case 0 => "zero"  // vzor overujici literal
    case Point(x, y) =>  // vzor overujici soucinovy typ
// Fuzz vlozeny komentar
      val sum = x + y /* inline */
      val text = toStr(sum)
      "point sum: " + text  // posledni radek je vysledek vetve

    case unit: Unit => "invalid input"  // vzor overujici pouze typ /* inline */
    case _ => "negative integer"  // vychozi vetev

// Fuzz vlozeny komentar
def describeAmount(value: Amount): String =
  value match
    case n_fuzz31: Double =>

      val text = toStr(n_fuzz31)
      "amount: " + text
    case _ => "invalid amount" /* inline */
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar

@main def algebdtExample(): Unit =
  val input: Input = readIntMaybe()
  val first = describe(input)
// Fuzz vlozeny komentar
  val point = Point(2, 3)
// Fuzz vlozeny komentar
  val second = describe(point)
  val amount: Amount = readDoubleMaybe()
  val third = describeAmount(amount)
// Fuzz vlozeny komentar

  print(first) /* inline */
  print("\n")
  print(second)
  print("\n")
  print(third)
  print("\n")
