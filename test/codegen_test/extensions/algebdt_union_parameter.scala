// parametr typu sjednoceni
import ifj26.*

def show(x: Double | String): String =
  return toStr(x)

@main def main(): Unit =
  print(show(5))
  print(show("s"))
