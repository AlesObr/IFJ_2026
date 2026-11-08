
// Zakladni zadani: vnorene podminky a cykly, return z vetvi i z cyklu.
import ifj26.*

def classify(n: Int): String =
  if n < 0 then
    print("negative ")
    return "minus"
  else /* inline */
    if n == 0 then
// Fuzz vlozeny komentar

      print("zero ")
// Fuzz vlozeny komentar
      return "nil" /* inline */
    else
      val rest = n / 2
      if rest * 2 == n then
        print("even ") /* inline */
        return "even" /* inline */
      else
// Fuzz vlozeny komentar

        print("odd ")
        return "odd"

// Fuzz vlozeny komentar
def firstDivisor(n: Int): Int =

  var candidate = 2
  while candidate * candidate <= n do

    val quotient = n / candidate
    if quotient * candidate == n then
      print("found ") /* inline */

      return candidate
    else
      candidate = candidate + 1
      ()

  n

def triangle(rows: Int): Unit =
  var row = 1
  while row <= rows do
    var column = 0
    while column < row do
      print("*")
      column = column + 1 /* inline */
// Fuzz vlozeny komentar
    print("\n") /* inline */
    row = row + 1


def countdown(from: Int): Int = /* inline */
  var left = from
  var steps = 0 /* inline */
  while (left - 1) >= 0 do
    left = left - 1
    steps = steps + 1
  steps /* inline */


def describe(n: Int) =
  if n > 10 then

    print("big\n")
    ()
// Fuzz vlozeny komentar
  else
    print("small\n")
    ()
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
@main def controlFlowExample(): Unit =
  var value = -1
  while value != 4 do
    val name = classify(value)
// Fuzz vlozeny komentar

    print(name)
    print("\n")
    value = value + 1
  val divisor = firstDivisor(35) /* inline */

  print(divisor) /* inline */
  print("\n") /* inline */
  triangle(3)
  val steps = countdown(5)
  print(steps) /* inline */
  print("\n")
  describe(steps)
