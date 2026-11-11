// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze. /* inline */
import ifj26.*


def factorialIterative(n: Int): Int =
// Fuzz vlozeny komentar
  var result = 1  // promenne /* inline */
  var i_fuzz25: Int = 2
  while i_fuzz25 <= n do /* inline */
    result = result * i_fuzz25
    i_fuzz25 = i_fuzz25 + 1
  result /* inline */

// Fuzz vlozeny komentar

def factorialRecursive(n: Int): Int =
  if n <= 1 then
    return 1
  else

    val previous = n - 1

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
    if n > 12 then /* inline */
// Fuzz vlozeny komentar
      print("Use an integer from 0 to 12.\n") /* inline */
// Fuzz vlozeny komentar
    else

      val iterative = factorialIterative(n) /* inline */
      val recursive = factorialRecursive(n)
      print("iterative: ")

      print(iterative)
// Fuzz vlozeny komentar
      print("\nrecursive: ")
      print(recursive)
      print("\n")
