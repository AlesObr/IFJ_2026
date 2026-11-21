// OK (0): 2.2: stejne jmeno ve dvou vetvich
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

@main def main(): Unit =
  if 1 < 2 then
    val t = 1
    print(t)
  else
    val t = 2
    print(t)
