// Rozsireni INTERPOLATEDS /* inline */
import ifj26.*

@main def interpolatedsExample(): Unit =
  val course = "IFJ" /* inline */
  val year = 26
  val count = 2
  val message = s"$course$year: ${count + 1} examples cost $$0."
  val multiline_fuzz54 = s"""He said "hello", $course.
Next year: ${year + 1}."""
  print(message)

  print("\n")
  print(multiline_fuzz54)
  print("\n") /* inline */
