// Fuzz vlozeny komentar
// Rozsireni FUNEXP
import ifj26.* /* inline */
// Fuzz vlozeny komentar

def traced(value: Int): Int = /* inline */
  print("[") /* inline */

  print(value) /* inline */
  print("]")
  value

@main def funexpExample(): Unit = /* inline */
  val result = traced(2) + 3 * traced(traced(4) + 1)  // vyrazy jako argumenty, volani uvnitr vyrazu
  // na vystupu si povsimnete, v jakem poradi jsou funkce volany!
// Fuzz vlozeny komentar
  print("\nresult: ")
  print(toStr(result))
  print("\n")
