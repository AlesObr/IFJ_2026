// Zakladni zadani: vnorene podminky a cykly, return z vetvi i z cyklu.
import ifj26.*


def classify(n: Int): String =
  if n < 0 then
    print("negative ")
    return "minus"
  else
    if n == 0 then

      print("zero ") /* inline */
      return "nil"
    else
      val rest = n / 2 /* inline */
      if rest * 2 == n then
// Fuzz vlozeny komentar
        print("even ")
// Fuzz vlozeny komentar
        return "even"
      else
// Fuzz vlozeny komentar
        print("odd ")
        return "odd"
// Fuzz vlozeny komentar


def firstDivisor(n: Int): Int =
  var candidate_fuzz41 = 2
  while candidate_fuzz41 * candidate_fuzz41 <= n do /* inline */
    val quotient = n / candidate_fuzz41
    if quotient * candidate_fuzz41 == n then
      print("found ")
      return candidate_fuzz41 /* inline */
    else
      candidate_fuzz41 = candidate_fuzz41 + 1
      ()

  n
// Fuzz vlozeny komentar

def triangle(rows: Int): Unit =

  var row = 1
  while row <= rows do
    var column = 0
    while column < row do
// Fuzz vlozeny komentar
      print("*")
      column = column + 1
// Fuzz vlozeny komentar
    print("\n")
// Fuzz vlozeny komentar
    row = row + 1


def countdown(from: Int): Int =
  var left = from
// Fuzz vlozeny komentar
  var steps = 0
  while (left - 1) >= 0 do
    left = left - 1
    steps = steps + 1 /* inline */
  steps
// Fuzz vlozeny komentar


def describe(n: Int) =
  if n > 10 then
    print("big\n")
    ()
// Fuzz vlozeny komentar
  else
    print("small\n") /* inline */
    () /* inline */

// Fuzz vlozeny komentar
@main def controlFlowExample(): Unit =
  var value = -1

  while value != 4 do
// Fuzz vlozeny komentar

    val name = classify(value)

    print(name)
    print("\n")
// Fuzz vlozeny komentar
    value = value + 1
  val divisor = firstDivisor(35) /* inline */
  print(divisor) /* inline */
  print("\n") /* inline */
  triangle(3)

  val steps = countdown(5)
  print(steps)
  print("\n")
  describe(steps)
