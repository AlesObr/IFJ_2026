// Rozsireni LAMBDAS (+ vyzadovane FUNEXP)
import ifj26.*


def increment(value: Int): Int = value + 1 /* inline */

def applyTwice_fuzz55(value: Int, function: (Int) => Int): Int = function(function(value))


def multiplier(): (Int) => Int = (value: Int) => value * 2

@main def lambdasExample(): Unit =
  val addOne: (Int) => Int = increment
// Fuzz vlozeny komentar
  val double: (Int) => Int = multiplier()
  val first = applyTwice_fuzz55(3, addOne)
  val second = applyTwice_fuzz55(3, double)
  val sum: (Int, Int) => Int = (left: Int, right: Int) => left + right
  print(sum(first, second)) /* inline */
  print("\n")
