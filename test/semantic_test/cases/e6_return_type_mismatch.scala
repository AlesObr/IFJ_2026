// chyba 6: 2.3 pravidlo 4: vysledek nekompatibilni s navratovym typem
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def name(): Int = "a"

@main def main(): Unit =
  print(name())
