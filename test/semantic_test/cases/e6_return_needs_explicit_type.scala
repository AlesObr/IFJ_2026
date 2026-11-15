// chyba 6: 2.3: return v tele vyzaduje explicitni navratovy typ
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def f(x: Int) =
  return x

@main def main(): Unit =
  print(f(1))
