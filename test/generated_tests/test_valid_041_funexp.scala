// Fuzz vlozeny komentar
// Rozsireni FUNEXP
import ifj26.*

def traced_fuzz32(value: Int): Int =
  print("[")
// Fuzz vlozeny komentar
  print(value)
  print("]")
  value

@main def funexpExample(): Unit =
  val result = traced_fuzz32(2) + 3 * traced_fuzz32(traced_fuzz32(4) + 1)  // vyrazy jako argumenty, volani uvnitr vyrazu
  // na vystupu si povsimnete, v jakem poradi jsou funkce volany!
  print("\nresult: ")
  print(toStr(result))
  print("\n")
