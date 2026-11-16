// funkce s typem Unit vrati () i pri return s hodnotou
import ifj26.*

def ignore(): Unit =
  return 5

def ignore2(): Unit = 7

@main def main(): Unit =
  val a = ignore()
  val b = ignore2()
  print(a)
  print(b)
  print("\n")
