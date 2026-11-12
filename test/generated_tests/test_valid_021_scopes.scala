// Zakladni zadani: rozsahy platnosti a prekryvani promennych v podblocich.
import ifj26.*


def shadowing(value: Int): Int =
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

  print(total) /* inline */
  print(" ")

  total = total + 1
  total



def loopScope(limit: Int): Unit =
  var index = 0
  while index < limit do /* inline */
    val value = index * 2
// Fuzz vlozeny komentar
    if value > 0 then
      val value = "positive"
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
  print(index)
  print("\n")

def sameNameAsFunction(shadowing: Int): Int =
  val loopScope = shadowing + 1
  loopScope * 2
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar

@main def scopesExample_fuzz33(): Unit =

  val value = 1 /* inline */
  val result = shadowing(value)
  print(result)
  print("\n")
// Fuzz vlozeny komentar
  loopScope(2)

  val other = sameNameAsFunction(value)
  print(other)
// Fuzz vlozeny komentar
  print("\n")
