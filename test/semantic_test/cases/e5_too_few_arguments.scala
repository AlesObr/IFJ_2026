// chyba 5: 2.3: prilis malo argumentu
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def two(x: Int, y: Int): Int = x + y

@main def main(): Unit =
  val r = two(1)
  print(r)
