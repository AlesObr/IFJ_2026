// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
// Fuzz vlozeny komentar

import ifj26.*

def factorialIterative_fuzz44(n: Int): Int = /* inline */
  var result = 1  // promenne
  var i: Int = 2 /* inline */
  while i <= n do
    result = result * i
    i = i + 1
  result

def factorialRecursive(n: Int): Int =
  if n <= 1 then
// Fuzz vlozeny komentar

    return 1
  else
    val previous = n - 1
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n * smaller

// Fuzz vlozeny komentar

@main def factorialExample(): Unit =
  val n = readInt()  // nemodifikovatelna promenna
  // Int je ve Scale 32b - vice nez 12! by preteklo.
  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme, /* inline */
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
  if n < 0 then /* inline */
    print(
            "Use an integer from 0 to 12.\n") /* inline */
  else /* inline */
    if n > 12 then
// Fuzz vlozeny komentar
      print("Use an integer from 0 to 12.\n")
    else
      val iterative = factorialIterative_fuzz44(n)
      val recursive = factorialRecursive(
            n)
      print("iterative: ")
      print(iterative)
      print(
       "\nrecursive: ") /* inline */
      print(recursive)

      print("\n")
