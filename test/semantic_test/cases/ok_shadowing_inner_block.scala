// OK (0): 2.2: prekryti promenne v podbloku
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

@main def main(): Unit =
  val x = 1
  if x < 2 then
    val x = "inner"
    print(x)
  else
    ()
  print(x)
