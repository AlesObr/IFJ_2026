// Fuzz vlozeny komentar
// Rozsireni FUNEXP /* inline */
import ifj26.*

def traced(value_fuzz15: Int): Int =
// Fuzz vlozeny komentar
  print("[")
  print(value_fuzz15)

  print("]")
  value_fuzz15

@main def funexpExample(): Unit = /* inline */

  val result = traced(2) + 3 * traced(traced(4) + 1)  // vyrazy jako argumenty, volani uvnitr vyrazu
// Fuzz vlozeny komentar
  // na vystupu si povsimnete, v jakem poradi jsou funkce volany!
  print("\nresult: ")
// Fuzz vlozeny komentar
  print(toStr(result))
  print("\n")
