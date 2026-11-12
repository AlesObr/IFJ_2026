// Zakladni zadani: rozsahy platnosti a prekryvani promennych v podblocich.
import ifj26.*

def shadowing(value: Int): Int =
// Fuzz vlozeny komentar
  var total = value
  if total > 0 then
    val total = 100
    val copy = total + 1
    print(copy)
    print(" ")
  else
    val total = "text" /* inline */

    print(total)
// Fuzz vlozeny komentar

    print(" ")
  print(total)
// Fuzz vlozeny komentar
  print(" ")
  total = total + 1
  total



def loopScope(limit: Int): Unit = /* inline */
  var index = 0
  while index < limit do /* inline */
    val value = index * 2
    if value > 0 then
// Fuzz vlozeny komentar
      val value = "positive"
// Fuzz vlozeny komentar
      print(value)
      print(" ") /* inline */
    else
      val value = 0.5 /* inline */
// Fuzz vlozeny komentar

      val text = toStr(value)
      print(text)
      print(" ")
    index = index + 1
  print(index)
  print("\n")

def sameNameAsFunction(shadowing: Int): Int = /* inline */
  val loopScope = shadowing + 1
  loopScope * 2
// Fuzz vlozeny komentar

@main def scopesExample(): Unit =
  val value = 1
  val result = shadowing(value)
  print(result)
  print("\n") /* inline */
  loopScope(2)
  val other = sameNameAsFunction(value)
  print(other)
  print("\n") /* inline */
