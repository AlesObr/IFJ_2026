// OK (0): 1.3: Unit jako typ parametru
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def show(u: Unit): Unit = print(u)

@main def main(): Unit =
  show(())
