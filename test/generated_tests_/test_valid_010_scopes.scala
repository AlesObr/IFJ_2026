// Zakladni zadani: rozsahy platnosti a prekryvani promennych v podblocich.
import ifj26.*

// Fuzz vlozeny komentar
def shadowing(value: Int): Int =
  var total = value
  if total > 0 then
    val total = 100
    val copy = total + 1
    print(copy)
    print(" ")
  else /* inline */
    val total = "text"
    print(total)
    print(" ")
  print(total)
  print(" ") /* inline */
  total = total + 1 /* inline */
  total
// Fuzz vlozeny komentar

def loopScope(limit_fuzz84: Int): Unit =
  var index = 0
  while index < limit_fuzz84 do /* inline */
    val value = index * 2
    if value > 0 then /* inline */
      val value = "positive"

      print(value)
      print(" ")
    else
      val value = 0.5
      val text = toStr(value) /* inline */
      print(text)
      print(" ")
    index = index + 1
  print(index) /* inline */

  print("\n")

def sameNameAsFunction(shadowing: Int): Int =
  val loopScope = shadowing + 1
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
