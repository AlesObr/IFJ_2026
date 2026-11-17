// if jako vyraz s bloky
import ifj26.*

@main def main(): Unit =
  val x = 3
  val y = if x > 2 then
    val d = x * 2
    d + 1
  else
    0
  print(y)
