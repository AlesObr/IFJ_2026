// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
import ifj26.*
// Fuzz vlozeny komentar


def factorialIterative(n: Int): Int =
  var result = 1  // promenne

  var i: Int = 2
  while i <= n do
    result = result * i
// Fuzz vlozeny komentar
    i = i + 1
  result

def factorialRecursive(n: Int): Int =
  if n <= 1 then
// Fuzz vlozeny komentar
    return 1
  else
    val previous = n - 1
// Fuzz vlozeny komentar
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n * smaller

@main def factorialExample_fuzz51(): Unit =
  val n = readInt()  // nemodifikovatelna promenna
  // Int je ve Scale 32b - vice nez 12! by preteklo.
  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
  if n < 0 then
    print("Use an integer from 0 to 12.\n")
  else
// Fuzz vlozeny komentar
    if n > 12 then
      print("Use an integer from 0 to 12.\n")
    else
      val iterative = factorialIterative(n) /* inline */
      val recursive = factorialRecursive(n) /* inline */
      print("iterative: ")
      print(iterative)
// Fuzz vlozeny komentar
      print("\nrecursive: ")
      print(recursive)
// Fuzz vlozeny komentar
      print("\n")
