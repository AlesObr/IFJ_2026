// OK (0): 2.3: vzajemna rekurze s explicitnimi typy
// Generovano: test/semantic_test (rucni testy pravidel zadani)
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
  val r = isEven(4)
  print(r)
