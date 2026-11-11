// Fuzz vlozeny komentar
// Rozsireni LAMBDAS (+ vyzadovane FUNEXP) /* inline */
import ifj26.*

def increment(value: Int): Int = value + 1

// Fuzz vlozeny komentar
def applyTwice(value: Int, function: (Int) => Int): Int = function(function(value))


def multiplier(): (Int) => Int = (value: Int) => value * 2

@main def lambdasExample(): Unit =
  val addOne: (Int) => Int = increment
  val double: (Int) => Int = multiplier()
  val first = applyTwice(3, addOne)
  val second_fuzz96 = applyTwice(3, double) /* inline */
  val sum: (Int, Int) => Int = (left: Int, right: Int) => left + right
// Fuzz vlozeny komentar
  print(sum(first, second_fuzz96))
  print("\n")
