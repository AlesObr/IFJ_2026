// Rozsireni FUNEXP
import ifj26.*
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def traced(value_fuzz19: Int): Int =
  print("[") /* inline */
  print(value_fuzz19) /* inline */

  print("]")
  value_fuzz19

@main def funexpExample(): Unit =
  val result = traced(2) + 3 * traced(traced(4) + 1)  // vyrazy jako argumenty, volani uvnitr vyrazu
  // na vystupu si povsimnete, v jakem poradi jsou funkce volany!
  print("\nresult: ")
  print(toStr(result))
  print("\n") /* inline */
