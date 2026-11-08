// Fuzz vlozeny komentar
// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
import ifj26.*


def factorialIterative(n: Int): Int =
  var result = 1  // promenne
  var i: Int = 2

  while i <= n do /* inline */
    result = result * i
// Fuzz vlozeny komentar
    i = i + 1

  result

def factorialRecursive(n: Int): Int = /* inline */
  if n <= 1 then
    return 1
  else

    val previous = n - 1
// Fuzz vlozeny komentar
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n * smaller

// Fuzz vlozeny komentar
@main def factorialExample(): Unit =
// Fuzz vlozeny komentar
  val n = readInt()  // nemodifikovatelna promenna
  // Int je ve Scale 32b - vice nez 12! by preteklo.
  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
  if n < 0 then
    print("Use an integer from 0 to 12.\n")
  else
    if n > 12 then /* inline */
      print("Use an integer from 0 to 12.\n")
    else
      val iterative = factorialIterative(n)
      val recursive = factorialRecursive(n)
      print("iterative: ")
      print(iterative) /* inline */
// Fuzz vlozeny komentar
      print("\nrecursive: ")
// Fuzz vlozeny komentar
      print(recursive)
      print("\n")
