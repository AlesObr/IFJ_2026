// Rozsireni LAMBDAS (+ vyzadovane FUNEXP)
import ifj26.* /* inline */
// Fuzz vlozeny komentar

def increment(value: Int): Int = value + 1

def applyTwice(value: Int, function: (Int) => Int): Int = function(function(value))

def multiplier(): (Int) => Int = (value: Int) => value * 2 /* inline */
// Fuzz vlozeny komentar



@main def lambdasExample(): Unit = /* inline */
  val addOne: (Int) => Int = increment /* inline */
  val double: (Int) => Int = multiplier()
  val first = applyTwice(3, addOne) /* inline */
  val second = applyTwice(3, double)
  val sum: (Int, Int) => Int = (left: Int, right: Int) => left + right
  print(sum(first, second))
  print("\n")
