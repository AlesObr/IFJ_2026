// OK (0): 2.3: volani pred lexikalni definici
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

@main def main(): Unit =
  val v = later(2)
  print(v)

def later(x: Int): Int = x * 2
