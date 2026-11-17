// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
import ifj26.*
// Fuzz vlozeny komentar

def factorialIterative(n_fuzz74: Int): Int =
  var result = 1  // promenne
// Fuzz vlozeny komentar
  var i: Int = 2
// Fuzz vlozeny komentar
  while i <= n_fuzz74 do
    result = result * i
    i = i + 1
  result

def factorialRecursive(n_fuzz74: Int): Int =
  if n_fuzz74 <= 1 then
// Fuzz vlozeny komentar
    return 1
  else
    val previous = n_fuzz74 - 1 /* inline */
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n_fuzz74 * smaller
// Fuzz vlozeny komentar

@main def factorialExample(): Unit = /* inline */
// Fuzz vlozeny komentar
  val n_fuzz74 = readInt()  // nemodifikovatelna promenna
  // Int je ve Scale 32b - vice nez 12! by preteklo.
  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
// Fuzz vlozeny komentar

  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.

  if n_fuzz74 < 0 then
    print("Use an integer from 0 to 12.\n")
  else
    if n_fuzz74 > 12 then
      print("Use an integer from 0 to 12.\n")
    else
      val iterative = factorialIterative(n_fuzz74) /* inline */
// Fuzz vlozeny komentar
      val recursive = factorialRecursive(n_fuzz74)
      print("iterative: ")
      print(iterative)
      print("\nrecursive: ")
      print(recursive)
      print("\n") /* inline */
