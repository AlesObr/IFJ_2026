// chyba 3: 2.2: promenna z tela cyklu mimo cyklus
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

@main def main(): Unit =
  var i = 0
  while i < 1 do
    val inner = i
    i = i + 1
  print(inner)
