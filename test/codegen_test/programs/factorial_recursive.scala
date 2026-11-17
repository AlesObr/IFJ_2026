// rekurze
import ifj26.*

def factorial(n: Int): Int =
  if n < 2 then
    return 1
  else
    val m = n - 1
    val sub = factorial(m)
    return n * sub

@main def main(): Unit =
  print(factorial(10))
  print("\n")
