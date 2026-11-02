// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
// Fuzz vložený komentář
import ifj26.*

def factorialIterative(n: Int): Int =
  var result = 1  // promenne

  var i: Int = 2

  while i <= n do
    result = result * i
    i = i + 1
  result

def factorialRecursive(n: Int): Int =

  if n <= 1 then
    return 1
  else
    val previous = n - 1
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n * smaller

@main def factorialExample(): Unit =
// Fuzz vložený komentář
  val n = readInt()  // nemodifikovatelna promenna /* inline */
// Fuzz vložený komentář
  // Int je ve Scale 32b - vice nez 12! by preteklo.
  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
// Fuzz vložený komentář
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
  if n < 0 then

    print("Use an integer from 0 to 12.\n")
// Fuzz vložený komentář
  else /* inline */
    if n > 12 then /* inline */
      print("Use an integer from 0 to 12.\n")
    else /* inline */
      val iterative = factorialIterative(n)

      val recursive = factorialRecursive(n)
      print("iterative: ")
      print(iterative)
      print("\nrecursive: ")

      print(recursive)
      print("\n")
