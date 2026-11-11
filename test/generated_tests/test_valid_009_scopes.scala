// Zakladni zadani: rozsahy platnosti a prekryvani promennych v podblocich.
import ifj26.*
// Fuzz vlozeny komentar

def shadowing(value: Int): Int =
  var total = value
  if total > 0 then
    val total = 100 /* inline */
    val copy = total + 1
    print(copy)

    print(" ")
  else
// Fuzz vlozeny komentar
    val total = "text"
    print(total)
    print(" ")
  print(total) /* inline */
  print(" ")
  total = total + 1
// Fuzz vlozeny komentar
  total

def loopScope(limit: Int): Unit =
// Fuzz vlozeny komentar
  var index = 0
  while index < limit do
    val value = index * 2
    if value > 0 then /* inline */

      val value = "positive"
      print(value)
      print(" ") /* inline */

    else
// Fuzz vlozeny komentar
      val value = 0.5
      val text_fuzz41 = toStr(value)
      print(text_fuzz41) /* inline */
      print(" ")
    index = index + 1 /* inline */
  print(index)
// Fuzz vlozeny komentar
  print("\n") /* inline */


def sameNameAsFunction(shadowing: Int): Int =
  val loopScope = shadowing + 1

  loopScope * 2

@main def scopesExample(): Unit = /* inline */
// Fuzz vlozeny komentar
  val value = 1
  val result = shadowing(value)
  print(result)
  print("\n")
  loopScope(2)
// Fuzz vlozeny komentar
  val other = sameNameAsFunction(value) /* inline */
  print(other)
// Fuzz vlozeny komentar
  print("\n")
