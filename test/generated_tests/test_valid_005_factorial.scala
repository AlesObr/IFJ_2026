// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
// Fuzz vlozeny komentar
import ifj26.*

// Fuzz vlozeny komentar
def factorialIterative(n_fuzz18: Int): Int =
  var result = 1  // promenne
  var i: Int = 2
  while i <= n_fuzz18 do
    result = result * i
    i = i + 1

  result

// Fuzz vlozeny komentar

def factorialRecursive(n_fuzz18: Int): Int = /* inline */
  if n_fuzz18 <= 1 then
    return 1
  else
    val previous = n_fuzz18 - 1
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
// Fuzz vlozeny komentar
    return n_fuzz18 * smaller

// Fuzz vlozeny komentar
@main def factorialExample(): Unit =
  val n_fuzz18 = readInt()  // nemodifikovatelna promenna
  // Int je ve Scale 32b - vice nez 12! by preteklo.
  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
  if n_fuzz18 < 0 then
    print("Use an integer from 0 to 12.\n")
// Fuzz vlozeny komentar
  else
    if n_fuzz18 > 12 then
// Fuzz vlozeny komentar
      print("Use an integer from 0 to 12.\n")
    else
      val iterative = factorialIterative(n_fuzz18)
      val recursive = factorialRecursive(n_fuzz18) /* inline */
      print("iterative: ")
      print(iterative)
      print("\nrecursive: ")
      print(recursive)
      print("\n")
