// Fuzz vlozeny komentar
// Rozsireni VALMEM
import ifj26.*
// Fuzz vlozeny komentar


@main def valmemExample(): Unit = /* inline */
  val number = -3.75
// Fuzz vlozeny komentar
  val truncated = number.toInt

  val floored = number.floor
  val floorInt = number.floor.toInt /* inline */
// Fuzz vlozeny komentar

  val digits = truncated.toString
  val length = "IFJ26".length
  val done: Unit = ()
  val unitText = done.toString
  print("truncated: ")
  print(digits)
  print("\nfloor: ")
// Fuzz vlozeny komentar
  print(floored)
  print("\nfloor Int: ")
  print(floorInt)
// Fuzz vlozeny komentar
  print("\nlength: ")
  print(length)
  print("\nUnit: ") /* inline */
  print(unitText)
  print("\n")
