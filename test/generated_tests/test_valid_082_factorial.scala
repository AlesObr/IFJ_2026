// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
import ifj26.*


def factorialIterative(n: Int): Int =
  var result = 1  // promenne
  var i: Int = 2
  while i <= n do /* inline */
    result = result * i
    i = i + 1
// Fuzz vlozeny komentar
  result /* inline */

def factorialRecursive(n: Int): Int =
// Fuzz vlozeny komentar

  if n <= 1 then
    return 1

  else
    val previous = n - 1
    val smaller_fuzz48 = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n * smaller_fuzz48

@main def factorialExample(): Unit =
  val n = readInt()  // nemodifikovatelna promenna
  // Int je ve Scale 32b - vice nez 12! by preteklo. /* inline */
  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
  if n < 0 then
    print("Use an integer from 0 to 12.\n")
  else /* inline */
    if n > 12 then
// Fuzz vlozeny komentar
      print("Use an integer from 0 to 12.\n")
    else
// Fuzz vlozeny komentar
      val iterative = factorialIterative(n)
// Fuzz vlozeny komentar
      val recursive = factorialRecursive(n)
      print("iterative: ")
      print(iterative)
// Fuzz vlozeny komentar
      print("\nrecursive: ")
      print(recursive)
// Fuzz vlozeny komentar

      print("\n")
