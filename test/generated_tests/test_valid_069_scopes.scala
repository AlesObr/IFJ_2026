// Zakladni zadani: rozsahy platnosti a prekryvani promennych v podblocich.
import ifj26.*

// Fuzz vlozeny komentar
def shadowing(value: Int): Int =
// Fuzz vlozeny komentar
  var total = value

  if total > 0 then
    val total = 100
    val copy = total + 1 /* inline */
    print(copy)
    print(" ")

  else
    val total = "text"
// Fuzz vlozeny komentar

    print(total)
    print(" ")
  print(total)
  print(" ")
  total = total + 1
  total /* inline */

def loopScope(limit: Int): Unit =

  var index = 0
  while index < limit do
    val value = index * 2
    if value > 0 then
// Fuzz vlozeny komentar

      val value = "positive"
      print(value)
      print(" ")
    else
      val value = 0.5
      val text = toStr(value) /* inline */
      print(text)
      print(" ")
// Fuzz vlozeny komentar
    index = index + 1
  print(index)
  print("\n") /* inline */

def sameNameAsFunction(shadowing: Int): Int =
  val loopScope = shadowing + 1
// Fuzz vlozeny komentar
  loopScope * 2

@main def scopesExample(): Unit =
  val value = 1
  val result = shadowing(value)
  print(result)

  print("\n")
  loopScope(2)
  val other = sameNameAsFunction(value)
  print(other)
  print("\n")
