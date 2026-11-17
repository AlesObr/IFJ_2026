// funkce bez seznamu se vyhodnoti pri kazdem pouziti
import ifj26.*

def tick: Int =
  print("t")
  1

@main def main(): Unit =
  val a = tick
  val b = tick
  print(a + b)
