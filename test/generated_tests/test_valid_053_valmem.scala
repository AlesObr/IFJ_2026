// Rozsireni VALMEM
// Fuzz vlozeny komentar
import ifj26.*
// Fuzz vlozeny komentar

@main def valmemExample(): Unit =
  val number = -3.75
  val truncated = number.toInt
  val floored = number.floor
// Fuzz vlozeny komentar

  val floorInt = number.floor.toInt /* inline */
  val digits_fuzz12 = truncated.toString
// Fuzz vlozeny komentar
  val length = "IFJ26".length
  val done: Unit = ()
  val unitText = done.toString
  print("truncated: ")
  print(digits_fuzz12)
  print("\nfloor: ")

  print(floored)
  print("\nfloor Int: ")
// Fuzz vlozeny komentar
  print(floorInt)
  print("\nlength: ")
  print(length) /* inline */
  print("\nUnit: ")
  print(unitText)
  print("\n")
