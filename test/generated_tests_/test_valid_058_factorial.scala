
// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
import ifj26.*

def factorialIterative(n: Int): Int =
  var result = 1  // promenne
  var i: Int = 2
  while i <= n do
    result = result * i
    i = i + 1
  result
// Fuzz vlozeny komentar

def factorialRecursive(n: Int): Int =
  if n <= 1 then
    return 1
  else
    val previous = n - 1 /* inline */
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n * smaller


@main def factorialExample_fuzz45(): Unit =
// Fuzz vlozeny komentar
  val n = readInt()  // nemodifikovatelna promenna
// Fuzz vlozeny komentar
  // Int je ve Scale 32b - vice nez 12! by preteklo.
// Fuzz vlozeny komentar
  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
  if n < 0 then
    print("Use an integer from 0 to 12.\n")
  else
// Fuzz vlozeny komentar
    if n > 12 then
      print("Use an integer from 0 to 12.\n")

    else
      val iterative = factorialIterative(n)
      val recursive = factorialRecursive(n)
// Fuzz vlozeny komentar
      print("iterative: ")
      print(iterative) /* inline */

      print("\nrecursive: ")
// Fuzz vlozeny komentar
      print(recursive) /* inline */
      print("\n")
