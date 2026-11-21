// metoda s parametrem
import ifj26.*

extension (value: Int)
  def plus(other: Int): Int = value + other

@main def main(): Unit =
  val x: Int = 6
  print(x.plus(12))
