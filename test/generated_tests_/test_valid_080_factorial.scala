// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.

import ifj26.*

def factorialIterative(n: Int): Int =
  var result = 1  // promenne
  var i: Int = 2
  while i <= n do
    result = result * i

    i = i + 1
// Fuzz vlozeny komentar
  result


def factorialRecursive(n: Int): Int =

  if n <= 1 then
    return 1
// Fuzz vlozeny komentar
  else
    val previous = n - 1
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n * smaller

// Fuzz vlozeny komentar
@main def factorialExample(): Unit = /* inline */
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
// Fuzz vlozeny komentar

      val recursive = factorialRecursive(n)
      print("iterative: ")

      print(iterative)
      print("\nrecursive: ")
      print(recursive) /* inline */
      print("\n")
