// Zakladni zadani: vstup, val/var, podminky, cyklus_fuzz15, funkce, rekurze.
// Fuzz vložený komentář
import ifj26.*

def factorialIterative(n: Int): Int =
  var result = 1  // promenne /* inline */
  var i: Int = 2 /* inline */
  while i <= n do
    result = result * i /* inline */
// Fuzz vložený komentář
    i = i + 1
  result

// Fuzz vložený komentář
def factorialRecursive(n: Int): Int =
  if n <= 1 then /* inline */
    return 1
  else
    val previous = n - 1 /* inline */

    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n * smaller


@main def factorialExample(): Unit =
  val n = readInt()  // nemodifikovatelna promenna
  // Int je ve Scale 32b - vice nez 12! by preteklo.
  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.

  if n < 0 then
    print("Use an integer from 0 to 12.\n")
  else
    if n > 12 then
// Fuzz vložený komentář
      print("Use an integer from 0 to 12.\n")
    else /* inline */
// Fuzz vložený komentář
      val iterative = factorialIterative(n) /* inline */
// Fuzz vložený komentář
      val recursive = factorialRecursive(n) /* inline */
      print("iterative: ")
      print(iterative)
      print("\nrecursive: ")
      print(recursive)
      print("\n")
