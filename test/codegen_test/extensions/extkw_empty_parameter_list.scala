// metoda s prazdnym seznamem parametru
import ifj26.*

extension (value: Int)
  def foo(): String = "foo" + toStr(value)

@main def main(): Unit =
  val x: Int = 6
  print(x.foo())
