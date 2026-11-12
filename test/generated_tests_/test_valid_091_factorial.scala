// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
// Fuzz vlozeny komentar

import ifj26.*


def factorialIterative(n_fuzz53: Int): Int =
// Fuzz vlozeny komentar
  var result = 1  // promenne
  var i: Int = 2
  while i <= n_fuzz53 do /* inline */
    result = result * i
    i = i + 1
  result

def factorialRecursive(n_fuzz53: Int): Int =
  if n_fuzz53 <= 1 then
    return 1
// Fuzz vlozeny komentar
  else
    val previous = n_fuzz53 - 1 /* inline */
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n_fuzz53 * smaller
// Fuzz vlozeny komentar

@main def factorialExample(): Unit = /* inline */
  val n_fuzz53 = readInt()  // nemodifikovatelna promenna
  // Int je ve Scale 32b - vice nez 12! by preteklo.
  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
// Fuzz vlozeny komentar
  if n_fuzz53 < 0 then
    print("Use an integer from 0 to 12.\n")
  else
    if n_fuzz53 > 12 then
      print("Use an integer from 0 to 12.\n")
    else
// Fuzz vlozeny komentar
      val iterative = factorialIterative(n_fuzz53) /* inline */
      val recursive = factorialRecursive(n_fuzz53)
      print("iterative: ")
// Fuzz vlozeny komentar
      print(iterative) /* inline */
      print("\nrecursive: ")
      print(recursive)
      print("\n")
