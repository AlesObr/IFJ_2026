// OK (0): 2.2: definice v tele cyklu (kazda iterace nova)
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

@main def main(): Unit =
  var i = 0
  while i < 3 do
    val sq = i * i
    print(sq)
    i = i + 1
