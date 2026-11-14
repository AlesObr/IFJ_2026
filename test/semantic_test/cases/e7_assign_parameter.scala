// chyba 7: 2.3: prirazeni do formalniho parametru
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def f(p: Int): Int =
  p = 2
  p

@main def main(): Unit =
  print(f(1))
