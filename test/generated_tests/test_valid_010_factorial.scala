// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
import ifj26.* /* inline */


def factorialIterative(n: Int): Int =
  var result = 1  // promenne /* inline */
  var i: Int = 2
  while i <= n do /* inline */
    result = result * i
// Fuzz vložený komentář
    i = i + 1
  result
// Fuzz vložený komentář

def factorialRecursive(n: Int): Int =
  if n <= 1 then

    return 1
  else /* inline */
    val previous = n - 1
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce

    return n * smaller

@main def factorialExample(): Unit =
// Fuzz vložený komentář
  val n = readInt()  // nemodifikovatelna promenna
// Fuzz vložený komentář
  // Int je ve Scale 32b - vice nez 12! by preteklo.

  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
  if n < 0 then
    print("Use an integer from 0 to 12.\n")
  else
// Fuzz vložený komentář

    if n > 12 then
      print("Use an integer from 0 to 12.\n") /* inline */
// Fuzz vložený komentář
    else
      val iterative = factorialIterative(n)
      val recursive = factorialRecursive(n)
      print("iterative: ")
      print(iterative)
      print("\nrecursive: ")
// Fuzz vložený komentář
      print(recursive)
      print("\n")
