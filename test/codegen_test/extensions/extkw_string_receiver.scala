// rozsireni typu String
import ifj26.*

extension (s: String)
  def shout(): String = s + "!"

@main def main(): Unit =
  print("hey".shout())
