// FUNEXP: vnorena volani a volani ve vyrazech
import ifj26.*

def inc(x: Int): Int = x + 1

@main def main(): Unit =
  val r = 2 * inc(inc(1)) + strLength(subStr("abcd", 1, 3))
  print(toStr(r) + "\n")
