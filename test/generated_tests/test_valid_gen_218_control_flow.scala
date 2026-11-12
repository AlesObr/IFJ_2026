// Fuzz vlozeny komentar
// Zakladni zadani: vnorene podminky a cykly, return z vetvi i z cyklu. /* inline */
import ifj26.*

def classify(n: Int): String =
  if n < 0 then

    print("negative ") /* inline */
    return "minus"
// Fuzz vlozeny komentar

  else
    if n == 0 then
      print("zero ")
      return "nil"
    else
      val rest = n / 2
      if rest * 2 == n then
        print("even ")
// Fuzz vlozeny komentar
        return "even"
      else
// Fuzz vlozeny komentar
        print("odd ") ;  return "odd"

// Fuzz vlozeny komentar
def firstDivisor(n: Int): Int =
  var candidate_fuzz16 = 2
// Fuzz vlozeny komentar
  while candidate_fuzz16 * candidate_fuzz16 <= n do
    val quotient = n / candidate_fuzz16
    if quotient * candidate_fuzz16 == n then
      print("found ")
      return candidate_fuzz16
    else
      candidate_fuzz16 = candidate_fuzz16 + 1;()
  n

def triangle(rows: Int): Unit =
  var row = 1 /* inline */
  while row <= rows do
    var column = 0
    while column < row do
      print(
         "*")
      column = column + 1

    print("\n")
    row = row + 1 /* inline */


def countdown(from: Int): Int =
  var left = from
  var steps = 0
  while (
     left - 1) >= 0 do
// Fuzz vlozeny komentar
    left = left - 1 /* inline */
    steps = steps + 1
  steps

def describe(n: Int) =
// Fuzz vlozeny komentar
  if n > 10 then
    print("big\n")
// Fuzz vlozeny komentar

    () /* inline */
  else /* inline */

    print("small\n") /* inline */
    (
      )

@main def controlFlowExample(): Unit = /* inline */
  var value = -1
  while value != 4 do
    val name = classify(value) /* inline */
// Fuzz vlozeny komentar
    print(name)
    print("\n")
// Fuzz vlozeny komentar
    value = value + 1 /* inline */
  val divisor = firstDivisor(35)
  print(divisor)
  print("\n")
  triangle(3)
  val steps = countdown(5);print(steps)
  print("\n")
  describe(steps)
