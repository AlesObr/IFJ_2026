// chyba 4: 2.2: opakovana definice ve stejnem bloku
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

@main def main(): Unit =
  val x = 1
  val x = 2
  print(x)
