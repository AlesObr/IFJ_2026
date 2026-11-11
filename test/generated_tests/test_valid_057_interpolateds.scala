// Rozsireni INTERPOLATEDS
import ifj26.*

// Fuzz vlozeny komentar
@main def interpolatedsExample(): Unit =
  val course = "IFJ"
// Fuzz vlozeny komentar
  val year = 26 /* inline */
  val count = 2

  val message = s"$course$year: ${count + 1} examples cost $$0." /* inline */

  val multiline = s"""He said "hello", $course.
Next year: ${year + 1}."""
  print(message) /* inline */
  print("\n")
  print(multiline)
  print("\n") /* inline */
