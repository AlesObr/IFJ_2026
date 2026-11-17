// List.range a operator ++
import ifj26.*

@main def main(): Unit =
  val a = List.range(1, 4)
  val b = List(10) ++ a
  print(b)
