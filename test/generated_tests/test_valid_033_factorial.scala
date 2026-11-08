// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.

import ifj26.* /* inline */

def factorialIterative(n: Int): Int =
  var result = 1  // promenne
  var i: Int = 2
  while i <= n do
// Fuzz vlozeny komentar
    result = result * i /* inline */
    i = i + 1
// Fuzz vlozeny komentar
  result
// Fuzz vlozeny komentar

def factorialRecursive(n: Int): Int =
  if n <= 1 then
    return 1 /* inline */
  else
    val previous_fuzz92 = n - 1
    val smaller = factorialRecursive(previous_fuzz92)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n * smaller /* inline */

@main def factorialExample(): Unit =

  val n = readInt()  // nemodifikovatelna promenna /* inline */

  // Int je ve Scale 32b - vice nez 12! by preteklo.
// Fuzz vlozeny komentar
  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,

  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
  if n < 0 then /* inline */
    print("Use an integer from 0 to 12.\n")
// Fuzz vlozeny komentar
  else
    if n > 12 then
      print("Use an integer from 0 to 12.\n")
// Fuzz vlozeny komentar
    else
      val iterative = factorialIterative(n)
      val recursive = factorialRecursive(n) /* inline */
      print("iterative: ") /* inline */
      print(iterative)
      print("\nrecursive: ")
      print(recursive) /* inline */
      print("\n")
