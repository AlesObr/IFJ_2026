// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
// Fuzz vlozeny komentar
import ifj26.* /* inline */

// Fuzz vlozeny komentar
def factorialIterative(n: Int): Int = /* inline */
  var result = 1  // promenne
// Fuzz vlozeny komentar
  var i: Int = 2
// Fuzz vlozeny komentar
  while i <= n do
    result = result * i
    i = i + 1
  result
// Fuzz vlozeny komentar


def factorialRecursive(n: Int): Int =
  if n <= 1 then
    return 1
  else
    val previous = n - 1
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n * smaller

@main def factorialExample_fuzz32(): Unit =
  val n = readInt()  // nemodifikovatelna promenna
  // Int je ve Scale 32b - vice nez 12! by preteklo.
  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
  if n < 0 then /* inline */
    print("Use an integer from 0 to 12.\n")
  else
    if n > 12 then
      print("Use an integer from 0 to 12.\n")

    else
      val iterative = factorialIterative(n)
      val recursive = factorialRecursive(n)
      print("iterative: ") /* inline */
      print(iterative)
// Fuzz vlozeny komentar
      print("\nrecursive: ") /* inline */
      print(recursive)
      print("\n")
