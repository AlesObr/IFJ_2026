// Fuzz vlozeny komentar
// Zakladni zadani: rozsahy platnosti a prekryvani promennych v podblocich.
import ifj26.*

def shadowing(value: Int): Int =

  var total = value
  if total > 0 then
// Fuzz vlozeny komentar
    val total = 100
    val copy = total + 1
    print(copy)
    print(" ")
  else
    val total = "text"

    print(total)
    print(" ")
  print(total)
  print(" ")

  total = total + 1 /* inline */

  total

def loopScope(limit: Int): Unit =
  var index = 0
  while index < limit do
    val value = index * 2
    if value > 0 then
      val value = "positive" /* inline */
// Fuzz vlozeny komentar
      print(value)
// Fuzz vlozeny komentar
      print(" ")
    else
      val value = 0.5
// Fuzz vlozeny komentar
      val text = toStr(value)
      print(text)
      print(" ")

    index = index + 1
// Fuzz vlozeny komentar
  print(index)
  print("\n")

def sameNameAsFunction(shadowing: Int): Int =
  val loopScope = shadowing + 1
  loopScope * 2
// Fuzz vlozeny komentar

@main def scopesExample_fuzz87(): Unit =
  val value = 1
  val result = shadowing(value)
// Fuzz vlozeny komentar
  print(result)
  print("\n")
// Fuzz vlozeny komentar
  loopScope(2)
  val other = sameNameAsFunction(value)
  print(other)
  print("\n")
