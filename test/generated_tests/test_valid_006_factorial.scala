// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
import ifj26.*
// Fuzz vložený komentář

def factorialIterative(n: Int): Int =
// Fuzz vložený komentář
  var result = 1  // promenne /* inline */
  var i: Int = 2
  while i <= n do
    result = result * i
    i = i + 1
  result

def factorialRecursive(n: Int): Int =
  if n <= 1 then
    return 1
  else /* inline */
    val previous = n - 1
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce

    return n * smaller

@main def factorialExample(): Unit =
  val n = readInt()  // nemodifikovatelna promenna

  // Int je ve Scale 32b - vice nez 12! by preteklo.

  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
// Fuzz vložený komentář
  if n < 0 then
// Fuzz vložený komentář
    print("Use an integer from 0 to 12.\n") /* inline */
  else
    if n > 12 then
      print("Use an integer from 0 to 12.\n")
    else
      val iterative = factorialIterative(n)
      val recursive = factorialRecursive(n)
      print("iterative: ") /* inline */
      print(iterative)
      print("\nrecursive: ")
      print(recursive) /* inline */
      print("\n")
