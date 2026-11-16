// OK (0): 2.3 pravidlo 4: i return v Unit funkci se zahodi
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def ignore(): Unit =
  return 5

@main def main(): Unit =
  ignore()
