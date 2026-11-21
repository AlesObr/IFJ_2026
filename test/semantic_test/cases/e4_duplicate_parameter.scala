// chyba 4: 2.3: dva parametry stejneho jmena
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def f(a: Int, a: Int): Int = a

@main def main(): Unit =
  print(f(1, 2))
