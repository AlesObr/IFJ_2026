// Zakladni zadani: rozsahy platnosti a prekryvani promennych v podblocich.
import ifj26.*

def shadowing(value: Int): Int =
  var total = value
  if total > 0 then
    val total = 100
    val copy = total + 1
    print(copy)
// Fuzz vlozeny komentar

    print(" ")
  else
    val total = "text"
    print(total)
    print(" ")
// Fuzz vlozeny komentar
  print(total) /* inline */
  print(" ")
  total = total + 1
  total

def loopScope_fuzz81(limit: Int): Unit =
  var index = 0
  while index < limit do
    val value = index * 2
    if value > 0 then /* inline */
      val value = "positive" /* inline */
      print(value)
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
// Fuzz vlozeny komentar
  print("\n")



def sameNameAsFunction(shadowing: Int): Int =
  val loopScope_fuzz81 = shadowing + 1

  loopScope_fuzz81 * 2

@main def scopesExample(): Unit = /* inline */
  val value = 1
  val result = shadowing(value)
  print(result)
  print("\n")
  loopScope_fuzz81(2)
// Fuzz vlozeny komentar
  val other = sameNameAsFunction(value)
  print(other)
  print("\n") /* inline */
