// Fuzz vlozeny komentar
// Rozsireni FUNEXP
import ifj26.* /* inline */
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def traced(value: Int): Int = /* inline */
  print("[")
// Fuzz vlozeny komentar

  print(value) /* inline */
  print("]")
// Fuzz vlozeny komentar
  value
// Fuzz vlozeny komentar

@main def funexpExample(): Unit = /* inline */
  val result = traced(2) + 3 * traced(traced(4) + 1)  // vyrazy jako argumenty, volani uvnitr vyrazu /* inline */
  // na vystupu si povsimnete, v jakem poradi jsou funkce volany!
  print("\nresult: ")
  print(toStr(result))
  print("\n") /* inline */
