// vzajemna rekurze
import ifj26.*

def isEven(n: Int): Int =
  if n == 0 then
    return 1
  else
    val m = n - 1
    return isOdd(m)

def isOdd(n: Int): Int =
  if n == 0 then
    return 0
  else
    val m = n - 1
    return isEven(m)

@main def main(): Unit =
  print(isEven(10))
  print(isOdd(7))
  print("\n")
