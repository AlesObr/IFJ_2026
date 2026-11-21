// chyba 4: 2.3: dve definice stejne funkce
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def twice(x: Int): Int = x * 2

def twice(x: Int): Int = x + x

@main def main(): Unit =
  print(twice(1))
