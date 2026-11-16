// celociselne deleni nulou konci chybou interpretu 57
import ifj26.*

@main def main(): Unit =
  print("before\n")
  val zero = 0
  val x = 1 / zero
  print(x)
