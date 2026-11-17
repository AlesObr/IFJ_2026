// hluboka rekurze (1000 ramcu)
import ifj26.*

def depth(n: Int): Int =
  if n == 0 then
    return 0
  else
    val m = n - 1
    val d = depth(m)
    return d + 1

@main def main(): Unit =
  print(depth(1000))
  print("\n")
