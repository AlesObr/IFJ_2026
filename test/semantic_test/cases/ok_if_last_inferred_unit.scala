// OK (0): 2.3 pravidlo 3: if na konci, typ neuveden, vetve typu Unit
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def say(x: Int) =
  if x < 0 then
    print("neg")
  else
    print("pos")

@main def main(): Unit =
  say(1)
