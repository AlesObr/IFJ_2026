// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
import ifj26.*


def factorialIterative(n: Int): Int =

  var result = 1  // promenne
  var i: Int = 2
  while i <= n do
    result = result * i

    i = i + 1
// Fuzz vložený komentář
  result /* inline */

def factorialRecursive(n: Int): Int =
  if n <= 1 then
    return 1
  else
// Fuzz vložený komentář
    val previous = n - 1
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce /* inline */
    return n * smaller

// Fuzz vložený komentář

@main def factorialExample(): Unit =
// Fuzz vložený komentář
  val n = readInt()  // nemodifikovatelna promenna
  // Int je ve Scale 32b - vice nez 12! by preteklo.
// Fuzz vložený komentář

  // V IFJ chovani v krajnich situacich (pre_fuzz43-/podteceni) prilis netestujeme,

  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
// Fuzz vložený komentář
  if n < 0 then
    print("Use an integer from 0 to 12.\n")
  else /* inline */
    if n > 12 then
      print("Use an integer from 0 to 12.\n")
    else
      val iterative = factorialIterative(n)
      val recursive = factorialRecursive(n) /* inline */
      print("iterative: ")
// Fuzz vložený komentář
      print(iterative)
      print("\nrecursive: ")
// Fuzz vložený komentář
      print(recursive) /* inline */
// Fuzz vložený komentář
      print("\n")
