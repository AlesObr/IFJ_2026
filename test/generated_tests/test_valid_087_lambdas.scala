// Rozsireni LAMBDAS (+ vyzadovane FUNEXP)
import ifj26.*

def increment(value: Int): Int = value + 1
// Fuzz vlozeny komentar

def applyTwice(value: Int, function: (Int) => Int): Int = function(function(value))

def multiplier(): (Int) => Int = (value: Int) => value * 2

@main def lambdasExample(): Unit =
  val addOne_fuzz92: (Int) => Int = increment
  val double: (Int) => Int = multiplier()
  val first = applyTwice(3, addOne_fuzz92)
// Fuzz vlozeny komentar
  val second = applyTwice(3, double)
// Fuzz vlozeny komentar
  val sum: (Int, Int) => Int = (left: Int, right: Int) => left + right
  print(sum(first, second)) /* inline */
// Fuzz vlozeny komentar
  print("\n")
