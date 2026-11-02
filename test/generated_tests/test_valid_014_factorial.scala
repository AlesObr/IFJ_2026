// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
// Fuzz vlozeny komentar
import ifj26.*

def factorialIterative(n: Int): Int = /* inline */

  var result = 1  // promenne
  var i: Int = 2
  while i <= n do
    result = result * i /* inline */
    i = i + 1
// Fuzz vlozeny komentar

  result


def factorialRecursive(n: Int): Int =
  if n <= 1 then
    return 1
  else
    val previous = n - 1
    val smaller_fuzz80 = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce

    return n * smaller_fuzz80
// Fuzz vlozeny komentar


// Fuzz vlozeny komentar
@main def factorialExample(): Unit =
  val n = readInt()  // nemodifikovatelna promenna
  // Int je ve Scale 32b - vice nez 12! by preteklo.
  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
  if n < 0 then
    print("Use an integer from 0 to 12.\n")
  else
    if n > 12 then
      print("Use an integer from 0 to 12.\n")
    else
// Fuzz vlozeny komentar
      val iterative = factorialIterative(n)
      val recursive = factorialRecursive(n)
      print("iterative: ")
      print(iterative)
      print("\nrecursive: ")
      print(recursive)
      print("\n")
