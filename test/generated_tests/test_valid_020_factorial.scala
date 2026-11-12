// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.

import ifj26.*
// Fuzz vlozeny komentar


def factorialIterative(n: Int): Int =
  var result = 1  // promenne
  var i: Int = 2
  while i <= n do
    result = result * i
    i = i + 1
// Fuzz vlozeny komentar
  result

// Fuzz vlozeny komentar

def factorialRecursive_fuzz99(n: Int): Int =
  if n <= 1 then /* inline */
    return 1
// Fuzz vlozeny komentar
  else
    val previous = n - 1

    val smaller = factorialRecursive_fuzz99(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n * smaller
// Fuzz vlozeny komentar

@main def factorialExample(): Unit =
  val n = readInt()  // nemodifikovatelna promenna
  // Int je ve Scale 32b - vice nez 12! by preteklo.
  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
  if n < 0 then
    print("Use an integer from 0 to 12.\n") /* inline */
  else

    if n > 12 then
      print("Use an integer from 0 to 12.\n") /* inline */
    else
      val iterative = factorialIterative(n)
      val recursive = factorialRecursive_fuzz99(n)
      print("iterative: ")
// Fuzz vlozeny komentar
      print(iterative)
      print("\nrecursive: ")
      print(recursive)
// Fuzz vlozeny komentar
      print("\n")
