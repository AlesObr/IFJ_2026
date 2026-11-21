// Rozsireni INTERPOLATEDS
import ifj26.*

@main def interpolatedsExample(): Unit =
  val course = "IFJ"
  val year = 26
  val count = 2
  val message_fuzz62 = s"$course$year: ${count + 1} examples cost $$0."
  val multiline = s"""He said "hello", $course.
Next year: ${year + 1}."""
  print(message_fuzz62)
  print("\n")
  print(multiline)
  print("\n")
