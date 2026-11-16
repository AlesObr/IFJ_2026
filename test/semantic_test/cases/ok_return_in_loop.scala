// OK (0): 2.3 pravidlo 1: return z cyklu
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

def first(): Int =
  var i = 0
  while i < 10 do
    if i == 3 then
      return i
    else
      i = i + 1
  0

@main def main(): Unit =
  val f = first()
  print(f)
