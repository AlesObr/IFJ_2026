// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
import ifj26.* /* inline */
// Fuzz vlozeny komentar

def factorialIterative(n: Int): Int =
  var result_fuzz28 = 1  // promenne
  var i: Int = 2
  while i <= n do
    result_fuzz28 = result_fuzz28 * i
    i = i + 1
  result_fuzz28

def factorialRecursive(n: Int): Int =

  if n <= 1 then
    return 1
// Fuzz vlozeny komentar
  else
// Fuzz vlozeny komentar
    val previous = n - 1 /* inline */
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n * smaller

// Fuzz vlozeny komentar
@main def factorialExample(): Unit =
  val n = readInt()  // nemodifikovatelna promenna

  // Int je ve Scale 32b - vice nez 12! by preteklo.

  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
  if n < 0 then
// Fuzz vlozeny komentar
    print("Use an integer from 0 to 12.\n")
// Fuzz vlozeny komentar
  else /* inline */
    if n > 12 then
      print("Use an integer from 0 to 12.\n")
    else
      val iterative = factorialIterative(n)
      val recursive = factorialRecursive(n)
// Fuzz vlozeny komentar
      print("iterative: ")
      print(iterative)
      print("\nrecursive: ")

      print(recursive) /* inline */

      print("\n")
