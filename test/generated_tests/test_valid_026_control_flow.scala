// Zakladni zadani: vnorene podminky a cykly, return z vetvi i z cyklu.
// Fuzz vlozeny komentar
import ifj26.*

def classify(n: Int): String =
  if n < 0 then /* inline */
    print("negative ")
    return "minus"
  else
    if n == 0 then
      print("zero ")
// Fuzz vlozeny komentar
      return "nil"
    else
      val rest = n / 2
      if rest * 2 == n then
// Fuzz vlozeny komentar
        print("even ")

        return "even"
      else
        print("odd ")
// Fuzz vlozeny komentar
        return "odd"

def firstDivisor(n: Int): Int =
  var candidate = 2
// Fuzz vlozeny komentar
  while candidate * candidate <= n do
    val quotient = n / candidate
    if quotient * candidate == n then
      print("found ")
      return candidate
    else
      candidate = candidate + 1 /* inline */
      ()

  n

def triangle(rows: Int): Unit = /* inline */
  var row = 1
  while row <= rows do
    var column = 0 /* inline */
    while column < row do
      print("*")
// Fuzz vlozeny komentar
      column = column + 1
    print("\n")
    row = row + 1

def countdown(from: Int): Int = /* inline */
  var left = from
  var steps = 0
  while (left - 1) >= 0 do
    left = left - 1
    steps = steps + 1
  steps

// Fuzz vlozeny komentar
def describe(n: Int) =
  if n > 10 then
// Fuzz vlozeny komentar
    print("big\n")
// Fuzz vlozeny komentar
    () /* inline */
  else
    print("small\n")
// Fuzz vlozeny komentar
    ()


@main def controlFlowExample(): Unit =
  var value = -1
  while value != 4 do
    val name = classify(value)
    print(name)
    print("\n")
// Fuzz vlozeny komentar
    value = value + 1
  val divisor = firstDivisor(35)
  print(divisor) /* inline */
// Fuzz vlozeny komentar
  print("\n")
  triangle(3)
  val steps = countdown(5)
  print(steps)
  print("\n")
  describe(steps)
