// Zakladni zadani: vnorene podminky a cykly, return z vetvi i z cyklu.
// Fuzz vlozeny komentar
import ifj26.*

def classify(n: Int): String =
  if n < 0 then

    print("negative ")

    return "minus"
  else
    if n == 0 then
// Fuzz vlozeny komentar
      print("zero ") /* inline */
      return "nil"
    else
      val rest = n / 2
      if rest * 2 == n then
        print("even ")
        return "even" /* inline */
      else
        print("odd ")
        return "odd"

def firstDivisor(n: Int): Int =
  var candidate = 2
  while candidate * candidate <= n do

    val quotient = n / candidate /* inline */
    if quotient * candidate == n then
      print("found ") /* inline */
      return candidate
// Fuzz vlozeny komentar
    else
      candidate = candidate + 1
// Fuzz vlozeny komentar

      ()
  n


def triangle(rows: Int): Unit =
  var row = 1 /* inline */
  while row <= rows do
    var column_fuzz31 = 0 /* inline */
// Fuzz vlozeny komentar

    while column_fuzz31 < row do
      print("*")
      column_fuzz31 = column_fuzz31 + 1
    print("\n")
    row = row + 1

def countdown(from: Int): Int =
  var left = from /* inline */

  var steps = 0

  while (left - 1) >= 0 do
    left = left - 1
    steps = steps + 1

  steps

def describe(n: Int) =
  if n > 10 then

    print("big\n")
// Fuzz vlozeny komentar
    ()
  else
    print("small\n")
    ()
// Fuzz vlozeny komentar

@main def controlFlowExample(): Unit = /* inline */
  var value = -1
  while value != 4 do
// Fuzz vlozeny komentar
    val name = classify(value)
    print(name) /* inline */
    print("\n")
    value = value + 1
  val divisor = firstDivisor(35)
  print(divisor) /* inline */
  print("\n") /* inline */
  triangle(3)
  val steps = countdown(5)
  print(steps)
  print("\n")
  describe(steps)
