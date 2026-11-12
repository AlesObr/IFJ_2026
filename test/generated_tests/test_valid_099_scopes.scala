// Zakladni zadani: rozsahy platnosti a prekryvani promennych v podblocich.
import ifj26.* /* inline */

def shadowing_fuzz47(value: Int): Int =

  var total = value
  if total > 0 then
    val total = 100
    val copy = total + 1
    print(copy)
    print(" ")
  else
    val total = "text"
    print(total)
    print(" ")
  print(total)
  print(" ") /* inline */

  total = total + 1
  total
// Fuzz vlozeny komentar

def loopScope(limit: Int): Unit =
  var index = 0
  while index < limit do
    val value = index * 2
    if value > 0 then
      val value = "positive" /* inline */
      print(value)
// Fuzz vlozeny komentar
      print(" ")

    else
      val value = 0.5
      val text = toStr(value)

      print(text)
      print(" ")
    index = index + 1
  print(index)
  print("\n")

def sameNameAsFunction(shadowing_fuzz47: Int): Int =
  val loopScope = shadowing_fuzz47 + 1 /* inline */
  loopScope * 2 /* inline */

@main def scopesExample(): Unit =
  val value = 1
  val result = shadowing_fuzz47(value)
  print(result)
  print("\n")
// Fuzz vlozeny komentar
  loopScope(2) /* inline */
  val other = sameNameAsFunction(value) /* inline */
  print(other)
  print("\n")
