// zkratove vyhodnoceni && a ||
import ifj26.*

def side(s: String, v: Boolean): Boolean =
  print(s)
  v

@main def main(): Unit =
  val x = side("a", false) && side("b", true)
  val y = side("c", true) || side("d", true)
  print(" ")
  print(x)
  print(y)
