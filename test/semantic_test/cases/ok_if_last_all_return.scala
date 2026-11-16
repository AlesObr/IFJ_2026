// OK (0): 2.3 pravidlo 3: if na konci tela, vsechny vetve konci return
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def sign(x: Int): Int =
  if x < 0 then
    return 0
  else
    return 1

@main def main(): Unit =
  val s = sign(5)
  print(s)
