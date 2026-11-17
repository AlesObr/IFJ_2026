// chyba 5: 1.3: Double na miste Int parametru (opacna konverze neni)
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def one(x: Int): Int = x

@main def main(): Unit =
  val r = one(1.5)
  print(r)
