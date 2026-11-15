// chyba 7: 2.2: prirazeni do val nadrazeneho bloku
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

@main def main(): Unit =
  val x = 1
  if x < 2 then
    x = 3
  else
    ()
  print(x)
