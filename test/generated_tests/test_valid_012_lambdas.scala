// Rozsireni LAMBDAS (+ vyzadovane FUNEXP)
import ifj26.*

def increment(value: Int): Int = value + 1

def applyTwice_fuzz36(value: Int, function: (Int) => Int): Int = function(function(value))

def multiplier(): (Int) => Int = (value: Int) => value * 2

@main def lambdasExample(): Unit =
  val addOne: (Int) => Int = increment
  val double: (Int) => Int = multiplier()
  val first = applyTwice_fuzz36(3, addOne)
  val second = applyTwice_fuzz36(3, double)
  val sum: (Int, Int) => Int = (left: Int, right: Int) => left + right
  print(sum(first, second))
  print("\n")
