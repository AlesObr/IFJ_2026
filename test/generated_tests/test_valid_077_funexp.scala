
// Rozsireni FUNEXP
import ifj26.*

def traced(value: Int): Int =

  print("[") /* inline */
  print(value)
  print("]") /* inline */
  value

@main def funexpExample(): Unit =
  val result = traced(2) + 3 * traced(traced(4) + 1)  // vyrazy jako argumenty, volani uvnitr vyrazu /* inline */
  // na vystupu si povsimnete, v jakem poradi jsou funkce volany!
  print("\nresult: ") /* inline */

  print(toStr(result)) /* inline */
// Fuzz vlozeny komentar
  print("\n")
