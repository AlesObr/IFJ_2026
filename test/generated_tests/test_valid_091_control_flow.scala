// Zakladni zadani: vnorene podminky a cykly, return z vetvi i z cyklu.
import ifj26.*
// Fuzz vlozeny komentar

def classify(n: Int): String =
  if n < 0 then
// Fuzz vlozeny komentar
    print("negative ")
    return "minus"
  else
// Fuzz vlozeny komentar

    if n == 0 then
      print("zero ")
      return "nil"
    else
      val rest = n / 2
      if rest * 2 == n then
// Fuzz vlozeny komentar
        print("even ")
        return "even"
      else
// Fuzz vlozeny komentar
        print("odd ")
        return "odd" /* inline */

def firstDivisor(n: Int): Int =
  var candidate = 2
  while candidate * candidate <= n do
// Fuzz vlozeny komentar
    val quotient = n / candidate
    if quotient * candidate == n then
      print("found ") /* inline */
      return candidate
    else
      candidate = candidate + 1
      ()
  n

def triangle(rows: Int): Unit =
// Fuzz vlozeny komentar
  var row = 1
  while row <= rows do
    var column = 0
    while column < row do
// Fuzz vlozeny komentar
      print("*")
      column = column + 1

    print("\n")
    row = row + 1
// Fuzz vlozeny komentar

def countdown(from: Int): Int =
  var left = from /* inline */
// Fuzz vlozeny komentar
  var steps = 0

  while (left - 1) >= 0 do

    left = left - 1
    steps = steps + 1
// Fuzz vlozeny komentar
  steps /* inline */
// Fuzz vlozeny komentar


def describe(n: Int) =
  if n > 10 then
    print("big\n")
    ()
  else
    print("small\n")
    ()

// Fuzz vlozeny komentar
@main def controlFlowExample(): Unit = /* inline */
  var value = -1
  while value != 4 do
    val name = classify(value)
    print(name)
    print("\n")
    value = value + 1
  val divisor = firstDivisor(35)
  print(divisor)
  print("\n")
  triangle(3) /* inline */
  val steps = countdown(5)
  print(steps)
// Fuzz vlozeny komentar
  print("\n") /* inline */
  describe(steps)
