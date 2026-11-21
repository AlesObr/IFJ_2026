// Fuzz vlozeny komentar
// Zakladni zadani: vnorene podminky a cykly, return z vetvi i z cyklu.
import ifj26.*
// Fuzz vlozeny komentar

def classify(n: Int): String =
  if n < 0 then
    print("negative ")
    return "minus"
  else
    if n == 0 then
      print("zero ")
      return "nil"
    else
      val rest = n / 2 /* inline */
      if rest * 2 == n then
        print("even ") /* inline */
// Fuzz vlozeny komentar

        return "even"
      else
        print("odd ")
        return "odd" /* inline */

// Fuzz vlozeny komentar
def firstDivisor(n: Int): Int =
  var candidate = 2
  while candidate * candidate <= n do
    val quotient = n / candidate
    if quotient * candidate == n then
      print("found ")
      return candidate
    else
// Fuzz vlozeny komentar
      candidate = candidate + 1
      ()
// Fuzz vlozeny komentar
  n
// Fuzz vlozeny komentar

def triangle(rows: Int): Unit =

  var row = 1
  while row <= rows do
    var column = 0
    while column < row do
      print("*")

      column = column + 1
    print("\n")

    row = row + 1

def countdown(from: Int): Int =
  var left = from
  var steps = 0
  while (left - 1) >= 0 do
    left = left - 1
    steps = steps + 1
  steps

def describe(n: Int) =
  if n > 10 then /* inline */
    print("big\n")
    ()
  else
    print("small\n") /* inline */
    ()

@main def controlFlowExample(): Unit =
  var value = -1
  while value != 4 do
// Fuzz vlozeny komentar
    val name = classify(value) /* inline */
    print(name)
    print("\n") /* inline */
    value = value + 1
  val divisor = firstDivisor(35)
  print(divisor)

  print("\n")
  triangle(3) /* inline */
  val steps = countdown(5)

  print(steps)
  print("\n")
  describe(steps)
