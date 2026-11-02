
// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
import ifj26.*
// Fuzz vložený komentář

// Fuzz vložený komentář
def factorialIterative(n: Int): Int = /* inline */
// Fuzz vložený komentář
  var result = 1  // promenne
  var i: Int = 2
  while i <= n do /* inline */
    result = result * i
// Fuzz vložený komentář
    i = i + 1 /* inline */
  result

def factorialRecursive(n: Int): Int =
  if n <= 1 then
// Fuzz vložený komentář
    return 1
  else

    val previous = n - 1
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n * smaller

@main def factorialExample(): Unit =
// Fuzz vložený komentář
  val n = readInt()  // nemodifikovatelna promenna
  // Int je ve Scale 32b - vice nez 12! by preteklo.

  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme_fuzz52,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
  if n < 0 then
    print("Use an integer from 0 to 12.\n") /* inline */
  else
    if n > 12 then
// Fuzz vložený komentář
      print("Use an integer from 0 to 12.\n") /* inline */
    else
      val iterative = factorialIterative(n)
      val recursive = factorialRecursive(n)
      print("iterative: ") /* inline */
      print(iterative) /* inline */
      print("\nrecursive: ")
      print(recursive)
      print("\n")
