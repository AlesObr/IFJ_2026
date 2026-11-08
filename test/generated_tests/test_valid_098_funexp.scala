// Rozsireni FUNEXP
import ifj26.*


def traced_fuzz75(value: Int): Int =
// Fuzz vlozeny komentar
  print("[")
  print(value)
  print("]") /* inline */
  value
// Fuzz vlozeny komentar

@main def funexpExample(): Unit = /* inline */
// Fuzz vlozeny komentar
  val result = traced_fuzz75(2) + 3 * traced_fuzz75(traced_fuzz75(4) + 1)  // vyrazy jako argumenty, volani uvnitr vyrazu
  // na vystupu si povsimnete, v jakem poradi jsou funkce volany!
  print("\nresult: ") /* inline */
  print(toStr(result))
  print("\n")
