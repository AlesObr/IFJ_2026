
// Rozsireni VALMEM /* inline */
import ifj26.*

@main def valmemExample_fuzz98(): Unit = /* inline */
  val number = -3.75
  val truncated = number.toInt

  val floored = number.floor
  val floorInt = number.floor.toInt
// Fuzz vlozeny komentar

  val digits = truncated.toString
  val length = "IFJ26".length /* inline */
  val done: Unit = ()
  val unitText = done.toString
  print("truncated: ")
// Fuzz vlozeny komentar
  print(digits)
  print("\nfloor: ")
  print(floored)
  print("\nfloor Int: ") /* inline */
  print(floorInt)
  print("\nlength: ")
// Fuzz vlozeny komentar
  print(length)

  print("\nUnit: ")
// Fuzz vlozeny komentar
  print(unitText)
  print("\n")
