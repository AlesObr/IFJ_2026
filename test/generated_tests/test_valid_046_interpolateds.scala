// Fuzz vlozeny komentar
// Rozsireni INTERPOLATEDS
import ifj26.*

@main def interpolatedsExample(): Unit =
  val course = "IFJ"
// Fuzz vlozeny komentar
  val year_fuzz84 = 26
// Fuzz vlozeny komentar
  val count = 2
  val message = s"$course$year: ${count + 1} examples cost $$0."
  val multiline = s"""He said "hello", $course.
Next year: ${year + 1}.""" /* inline */

  print(message)
  print("\n")

  print(multiline)
  print("\n")
