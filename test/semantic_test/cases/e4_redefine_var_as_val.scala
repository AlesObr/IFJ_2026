// chyba 4: 2.2: var i val stejneho jmena ve stejnem bloku
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

@main def main(): Unit =
  var x = 1
  val x = 2
  print(x)
