// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
import ifj26.* /* inline */

// Fuzz vlozeny komentar
def factorialIterative(n: Int): Int =
  var result = 1  // promenne
// Fuzz vlozeny komentar
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

@main def factorialExample_fuzz36(): Unit =
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
      val iterative = factorialIterative(n)
      val recursive = factorialRecursive(n)
      print("iterative: ")
      print(iterative)
      print("\nrecursive: ") /* inline */

      print(recursive)
// Fuzz vlozeny komentar

      print("\n") /* inline */
