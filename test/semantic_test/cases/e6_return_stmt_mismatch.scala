// chyba 6: 2.3 pravidlo 1: return s nekompatibilni hodnotou
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def name(): Int =
  return "a"

@main def main(): Unit =
  print(name())
