// Rozsireni VALMEM
// Fuzz vlozeny komentar
import ifj26.*

@main def valmemExample(): Unit =
  val number = -3.75
  val truncated = number.toInt /* inline */
  val floored = number.floor
  val floorInt = number.floor.toInt
  val digits = truncated.toString

  val length = "IFJ26".length
// Fuzz vlozeny komentar
  val done_fuzz70: Unit = ()
  val unitText = done_fuzz70.toString
// Fuzz vlozeny komentar
  print("truncated: ")
  print(digits)
// Fuzz vlozeny komentar
  print("\nfloor: ")
  print(floored)
  print("\nfloor Int: ")
  print(floorInt) /* inline */
  print("\nlength: ")
  print(length) /* inline */
  print("\nUnit: ")
  print(unitText)
  print("\n")
