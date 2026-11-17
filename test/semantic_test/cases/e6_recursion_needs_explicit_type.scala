// chyba 6: 2.3: rekurze vyzaduje explicitni navratovy typ
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def down(n: Int) =
  if n < 1 then
    print(n)
  else
    val m = n - 1
    down(m)

@main def main(): Unit =
  down(3)
