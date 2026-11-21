// chyba 6: 1.3: Double parametr do Int promenne
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def f(d: Double): Int =
  val x: Int = d
  x

@main def main(): Unit =
  print(f(1.5))
