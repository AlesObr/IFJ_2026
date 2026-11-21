// Rozsireni FUNEXP
import ifj26.*

def traced_fuzz52(value: Int): Int =
  print("[")
  print(value)
  print("]")
  value

@main def funexpExample(): Unit =
  val result = traced_fuzz52(2) + 3 * traced_fuzz52(traced_fuzz52(4) + 1)  // vyrazy jako argumenty, volani uvnitr vyrazu
  // na vystupu si povsimnete, v jakem poradi jsou funkce volany!
  print("\nresult: ")
  print(toStr(result))
  print("\n")
