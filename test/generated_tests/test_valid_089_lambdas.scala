// Rozsireni LAMBDAS (+ vyzadovane FUNEXP)
import ifj26.*

// Fuzz vlozeny komentar
def increment_fuzz49(value: Int): Int = value + 1 /* inline */
// Fuzz vlozeny komentar

def applyTwice(value: Int, function: (Int) => Int): Int = function(function(value))

def multiplier(): (Int) => Int = (value: Int) => value * 2

// Fuzz vlozeny komentar
@main def lambdasExample(): Unit =
  val addOne: (Int) => Int = increment_fuzz49
// Fuzz vlozeny komentar
  val double: (Int) => Int = multiplier()
  val first = applyTwice(3, addOne)
  val second = applyTwice(3, double)
// Fuzz vlozeny komentar
  val sum: (Int, Int) => Int = (left: Int, right: Int) => left + right /* inline */
  print(sum(first, second))
  print("\n")
