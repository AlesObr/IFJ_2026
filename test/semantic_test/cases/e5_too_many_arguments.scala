// chyba 5: 2.3: prilis mnoho argumentu
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def one(x: Int): Int = x

@main def main(): Unit =
  val r = one(1, 2)
  print(r)
