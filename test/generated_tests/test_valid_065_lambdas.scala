// Rozsireni LAMBDAS (+ vyzadovane FUNEXP)
import ifj26.*
// Fuzz vlozeny komentar


def increment(value: Int): Int = value + 1 /* inline */

def applyTwice(value: Int, function: (Int) => Int): Int = function(function(value))

def multiplier(): (Int) => Int = (value: Int) => value * 2 /* inline */
// Fuzz vlozeny komentar


@main def lambdasExample(): Unit =
  val addOne: (Int) => Int = increment
  val double: (Int) => Int = multiplier()
  val first = applyTwice(3, addOne)
  val second = applyTwice(3, double)
  val sum: (Int, Int) => Int = (left: Int, right: Int) => left + right
// Fuzz vlozeny komentar
  print(sum(first, second))
  print("\n")
