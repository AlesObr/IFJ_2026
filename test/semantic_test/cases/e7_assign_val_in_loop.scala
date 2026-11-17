// chyba 7: 2.2: prirazeni do val v cyklu
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

@main def main(): Unit =
  val limit = 3
  var i = 0
  while i < limit do
    limit = 2
    i = i + 1
