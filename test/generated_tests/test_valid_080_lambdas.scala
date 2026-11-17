// Fuzz vlozeny komentar
// Rozsireni LAMBDAS (+ vyzadovane FUNEXP) /* inline */
import ifj26.*
// Fuzz vlozeny komentar

def increment(value: Int): Int = value + 1

def applyTwice(value: Int, function: (Int) => Int): Int = function(function(value))

def multiplier(): (Int) => Int = (value: Int) => value * 2


// Fuzz vlozeny komentar
@main def lambdasExample(): Unit =
  val addOne: (Int) => Int = increment
  val double_fuzz75: (Int) => Int = multiplier()
  val first = applyTwice(3, addOne)
  val second = applyTwice(3, double_fuzz75)
  val sum: (Int, Int) => Int = (left: Int, right: Int) => left + right
  print(sum(first, second))
// Fuzz vlozeny komentar
  print("\n")
