// chyba 3: 2.2: promenna z vetve if mimo jeji rozsah
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

@main def main(): Unit =
  if 1 < 2 then
    val y = 1
    print(y)
  else
    ()
  print(y)
