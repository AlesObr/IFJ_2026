// chyba 6: 2.3 pravidlo 3: if na konci Int funkce, ne vsechny vetve konci return
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def f(x: Int): Int =
  if x < 0 then
    return 0
  else
    print("x")

@main def main(): Unit =
  print(f(1))
