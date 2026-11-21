// OK (0): 2.2: opakovane prirazeni do var
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

@main def main(): Unit =
  var x = 1
  x = 2
  x = x + 1
  print(x)
