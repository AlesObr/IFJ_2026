// OK (0): 2.3 pravidlo 1: samostatny return ve funkci s Unit
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def early(x: Int): Unit =
  if x < 0 then
    return
  else
    print("x")
  print("end")

@main def main(): Unit =
  early(1)
