// OK (0): 1.3: konverze Int -> Double u argumentu
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def half(x: Double): Double = x / 2

@main def main(): Unit =
  val h = half(3)
  print(h)
