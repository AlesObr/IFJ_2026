// OK (0): 4: vsechny vestavene funkce se spravnymi typy
// Generovano: test/semantic_test (rucni testy pravidel zadani)
import ifj26.*

@main def main(): Unit =
  val line = readLine()
  val i = readInt()
  val d = readDouble()
  val s = toStr(d)
  val t = toInt(d)
  val n = strLength(s)
  val sub = subStr("abcd", 1, 3)
  val o = ord(sub, 0)
  val c = chr(o)
  print(line)
  print(i)
  print(t)
  print(n)
  print(c)
