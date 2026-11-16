// OK (0): 2.3: odvozeny navratovy typ
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def five() = 5

@main def main(): Unit =
  val x: Int = five()
  print(x)
