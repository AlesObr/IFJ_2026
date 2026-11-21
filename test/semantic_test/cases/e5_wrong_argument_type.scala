// chyba 5: 2.3: nekompatibilni typ argumentu
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def one(x: Int): Int = x

@main def main(): Unit =
  val r = one("s")
  print(r)
