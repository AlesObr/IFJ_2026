// Zakladni zadani: vnorene podminky a cykly, return z vetvi i z cyklu.
import ifj26.* /* inline */

def classify(n: Int): String =
  if n < 0 then
    print("negative ")
// Fuzz vlozeny komentar
    return "minus"

  else
    if n == 0 then /* inline */
      print("zero ")
      return "nil"
    else
      val rest = n / 2
      if rest * 2 == n then
        print("even ")
        return "even"
      else
// Fuzz vlozeny komentar
        print("odd ")
        return "odd"

def firstDivisor(n: Int): Int =
  var candidate = 2
  while candidate * candidate <= n do
// Fuzz vlozeny komentar
    val quotient = n / candidate /* inline */
    if quotient * candidate == n then

      print("found ")
      return candidate
    else
      candidate = candidate + 1

      ()
  n /* inline */
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def triangle(rows: Int): Unit =
  var row = 1
  while row <= rows do
// Fuzz vlozeny komentar
    var column = 0
// Fuzz vlozeny komentar
    while column < row do
      print("*")
      column = column + 1
    print("\n")
    row = row + 1

def countdown(from: Int): Int =
  var left = from
  var steps = 0
  while (left - 1) >= 0 do /* inline */
    left = left - 1
    steps = steps + 1
// Fuzz vlozeny komentar
  steps
// Fuzz vlozeny komentar

def describe(n: Int) =
  if n > 10 then
    print("big\n") /* inline */
    () /* inline */
  else

    print("small\n")
    ()
// Fuzz vlozeny komentar

@main def controlFlowExample(): Unit =
  var value = -1 /* inline */
  while value != 4 do
    val name = classify(value)
    print(name) /* inline */
    print("\n")
    value = value + 1
  val divisor = firstDivisor(35)
// Fuzz vlozeny komentar
  print(divisor)
  print("\n")
// Fuzz vlozeny komentar
  triangle(3) /* inline */
  val steps = countdown(5)
  print(steps)
  print("\n")
  describe(steps)
