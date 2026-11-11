
// Zakladni zadani: vnorene podminky a cykly, return z vetvi i z cyklu.
import ifj26.*

def classify(n: Int): String = /* inline */
  if n < 0 then
    print("negative ") /* inline */
    return "minus"
  else
    if n == 0 then
      print("zero ")
      return "nil"
    else /* inline */
// Fuzz vlozeny komentar
      val rest_fuzz31 = n / 2
      if rest_fuzz31 * 2 == n then

        print("even ") /* inline */
        return "even"
      else
        print("odd ")
// Fuzz vlozeny komentar
        return "odd"
// Fuzz vlozeny komentar

def firstDivisor(n: Int): Int =
  var candidate = 2
  while candidate * candidate <= n do
    val quotient = n / candidate
    if quotient * candidate == n then
      print("found ")
      return candidate /* inline */
    else
      candidate = candidate + 1
      ()
// Fuzz vlozeny komentar
  n /* inline */

def triangle(rows: Int): Unit =
  var row = 1
  while row <= rows do
    var column = 0
    while column < row do

      print("*") /* inline */
      column = column + 1
// Fuzz vlozeny komentar

    print("\n")
    row = row + 1
// Fuzz vlozeny komentar

def countdown(from: Int): Int =
  var left = from
  var steps = 0 /* inline */
  while (left - 1) >= 0 do
    left = left - 1
    steps = steps + 1
  steps /* inline */


def describe(n: Int) =
  if n > 10 then /* inline */
    print("big\n")
    ()
  else
    print("small\n")
    ()

@main def controlFlowExample(): Unit =
// Fuzz vlozeny komentar
  var value = -1
  while value != 4 do
    val name = classify(value)
// Fuzz vlozeny komentar
    print(name)

    print("\n")
    value = value + 1

  val divisor = firstDivisor(35) /* inline */
  print(divisor)

  print("\n")
  triangle(3) /* inline */
  val steps = countdown(5)
// Fuzz vlozeny komentar
  print(steps) /* inline */
  print("\n")
  describe(steps)
