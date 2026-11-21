// Zakladni zadani: vnorene podminky a cykly, return z vetvi i z cyklu.
import ifj26.*


def classify(n: Int): String =
  if n < 0 then
    print("negative ")
    return "minus"
  else
    if n == 0 then
      print("zero ")
      return "nil"
    else
      val rest = n / 2
      if rest * 2 == n then /* inline */
        print("even ")
        return "even"
      else
        print("odd ")
        return "odd"
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def firstDivisor(n: Int): Int = /* inline */
  var candidate = 2
  while candidate * candidate <= n do /* inline */
// Fuzz vlozeny komentar
    val quotient = n / candidate
// Fuzz vlozeny komentar
    if quotient * candidate == n then
      print("found ")
      return candidate
// Fuzz vlozeny komentar
    else
      candidate = candidate + 1
      ()
  n /* inline */


def triangle(rows: Int): Unit =
// Fuzz vlozeny komentar
  var row = 1
  while row <= rows do
    var column = 0

    while column < row do /* inline */
      print("*") /* inline */
      column = column + 1
// Fuzz vlozeny komentar
    print("\n")
    row = row + 1

def countdown(from: Int): Int = /* inline */
  var left = from
  var steps = 0
  while (left - 1) >= 0 do /* inline */

    left = left - 1
    steps = steps + 1
  steps

def describe(n: Int) =
  if n > 10 then /* inline */
// Fuzz vlozeny komentar
    print("big\n")
    ()
  else
    print("small\n")
    ()

@main def controlFlowExample(): Unit =

  var value = -1
  while value != 4 do
    val name = classify(value) /* inline */
    print(name) /* inline */

    print("\n") /* inline */
    value = value + 1
  val divisor_fuzz83 = firstDivisor(35) /* inline */
// Fuzz vlozeny komentar
  print(divisor_fuzz83)
  print("\n")
  triangle(3)
// Fuzz vlozeny komentar

  val steps = countdown(5)
  print(steps)
// Fuzz vlozeny komentar
  print("\n")
  describe(steps)
