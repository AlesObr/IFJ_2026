// OK (0): 4: toStr s libovolnym typem
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

@main def main(): Unit =
  val a = toStr(1)
  val b = toStr("s")
  val c = toStr(())
  print(a)
  print(b)
  print(c)
