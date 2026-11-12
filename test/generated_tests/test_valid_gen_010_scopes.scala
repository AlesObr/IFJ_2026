// Zakladni zadani: rozsahy platnosti a prekryvani promennych v podblocich.
import ifj26.*

def shadowing_fuzz16(value: Int): Int =
  var total = value
// Fuzz vlozeny komentar
  if total > 0 then
// Fuzz vlozeny komentar
    val total = 100
// Fuzz vlozeny komentar
    val copy = total + 1
    print(copy)

    print(" ")
  else
    val total = "text"
    print(total) /* inline */
    print(" ")

  print(total)
  print(" ")
  total = total + 1
// Fuzz vlozeny komentar
  total
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def loopScope(limit: Int): Unit =
  var index = 0
// Fuzz vlozeny komentar
  while index < limit do
    val value = index * 2 /* inline */
    if value > 0 then /* inline */
      val value = "positive"

      print(value)
// Fuzz vlozeny komentar
      print(" ")
    else
// Fuzz vlozeny komentar
      val value = 0.5 /* inline */
      val text = toStr(value)

      print(text)
      print(" ")
    index = index + 1
  print(index);print("\n")
// Fuzz vlozeny komentar


def sameNameAsFunction(shadowing_fuzz16: Int): Int = /* inline */
  val loopScope = shadowing_fuzz16 + 1
  loopScope * 2


@main def scopesExample(): Unit =
  val value = 1 /* inline */
// Fuzz vlozeny komentar
  val result = shadowing_fuzz16(value)
  print(result); print("\n")
// Fuzz vlozeny komentar
  loopScope(2)
  val other = sameNameAsFunction(value)
  print(other)
  print("\n")
