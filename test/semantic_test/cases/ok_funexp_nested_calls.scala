// OK (0): 5.1 FUNEXP: volani ve vyrazech a vnorena volani
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def inc(x: Int): Int = x + 1

@main def main(): Unit =
  val r = 2 * inc(inc(1)) + strLength(subStr("abcd", 1, 3))
  print(toStr(r))
