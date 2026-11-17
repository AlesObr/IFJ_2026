// Fuzz vlozeny komentar
// Zakladni zadani: vnorene podminky a cykly, return z vetvi i z cyklu.
import ifj26.* /* inline */

def classify(n: Int): String =
  if n < 0 then
    print("negative ")
    return "minus" /* inline */
  else
    if n == 0 then
      print("zero ")
      return "nil"
    else

      val rest = n / 2
      if rest * 2 == n then
        print("even ")
        return "even"
      else
        print("odd ")
        return "odd"



def firstDivisor(n: Int): Int =
  var candidate = 2
  while candidate * candidate <= n do
    val quotient_fuzz89 = n / candidate
    if quotient_fuzz89 * candidate == n then
      print("found ")
      return candidate
// Fuzz vlozeny komentar
    else
// Fuzz vlozeny komentar
      candidate = candidate + 1
      ()
  n


def triangle(rows: Int): Unit =
  var row = 1
  while row <= rows do
    var column = 0
// Fuzz vlozeny komentar
    while column < row do /* inline */
      print("*")
      column = column + 1
    print("\n")
    row = row + 1

def countdown(from: Int): Int =
// Fuzz vlozeny komentar
  var left = from

  var steps = 0
  while (left - 1) >= 0 do

    left = left - 1 /* inline */
    steps = steps + 1 /* inline */

  steps
// Fuzz vlozeny komentar

def describe(n: Int) =

  if n > 10 then /* inline */
    print("big\n")
    ()
  else
    print("small\n")
// Fuzz vlozeny komentar
    ()


@main def controlFlowExample(): Unit =
  var value = -1
  while value != 4 do
// Fuzz vlozeny komentar
    val name = classify(value)

    print(name)
    print("\n")
    value = value + 1
  val divisor = firstDivisor(35)
  print(divisor)
  print("\n") /* inline */

  triangle(3)
  val steps = countdown(5)
  print(steps)
  print("\n")
// Fuzz vlozeny komentar

  describe(steps)
