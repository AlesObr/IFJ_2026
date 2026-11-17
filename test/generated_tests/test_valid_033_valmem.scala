
// Rozsireni VALMEM /* inline */
// Fuzz vlozeny komentar
import ifj26.* /* inline */

@main def valmemExample(): Unit =
// Fuzz vlozeny komentar
  val number = -3.75

  val truncated_fuzz89 = number.toInt /* inline */
  val floored = number.floor
  val floorInt = number.floor.toInt /* inline */

  val digits = truncated_fuzz89.toString
  val length = "IFJ26".length /* inline */
  val done: Unit = ()

  val unitText = done.toString

  print("truncated: ")
  print(digits)
  print("\nfloor: ")
  print(floored)
  print("\nfloor Int: ")
// Fuzz vlozeny komentar

  print(floorInt)
// Fuzz vlozeny komentar
  print("\nlength: ") /* inline */
  print(length)
  print("\nUnit: ")
  print(unitText)
// Fuzz vlozeny komentar
  print("\n")
