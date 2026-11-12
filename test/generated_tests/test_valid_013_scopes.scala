// Zakladni zadani: rozsahy platnosti a prekryvani promennych v podblocich.

import ifj26.*
// Fuzz vlozeny komentar


def shadowing(value: Int): Int =
  var total_fuzz32 = value
  if total_fuzz32 > 0 then
    val total_fuzz32 = 100
    val copy = total_fuzz32 + 1

    print(copy) /* inline */

    print(" ")
  else
    val total_fuzz32 = "text" /* inline */

    print(total_fuzz32)
    print(" ")
  print(total_fuzz32) /* inline */

  print(" ")
  total_fuzz32 = total_fuzz32 + 1
  total_fuzz32 /* inline */
// Fuzz vlozeny komentar

def loopScope(limit: Int): Unit =
  var index = 0
  while index < limit do
    val value = index * 2
    if value > 0 then
      val value = "positive" /* inline */
// Fuzz vlozeny komentar
      print(value)
      print(" ")
// Fuzz vlozeny komentar
    else
      val value = 0.5

      val text = toStr(value)
// Fuzz vlozeny komentar
      print(text)
      print(" ") /* inline */
    index = index + 1
  print(index)
// Fuzz vlozeny komentar

  print("\n")


def sameNameAsFunction(shadowing: Int): Int =
  val loopScope = shadowing + 1
  loopScope * 2

@main def scopesExample(): Unit = /* inline */
  val value = 1 /* inline */
  val result = shadowing(value)
  print(result)
  print("\n")
// Fuzz vlozeny komentar
  loopScope(2)
  val other = sameNameAsFunction(value)
  print(other)
  print("\n")
