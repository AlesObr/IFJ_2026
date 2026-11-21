// OK (0): 2.3 pravidlo 3: if na konci tela funkce s typem Unit
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def say(x: Int): Unit =
  if x < 0 then
    print("neg")
  else
    print("pos")

@main def main(): Unit =
  say(1)
