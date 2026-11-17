// vyraz a volani jako argument
import ifj26.*

def add(a: Int, b: Int): Int = a + b

@main def main(): Unit =
  print(add(add(1, 2) * 2, strLength("abc")))
