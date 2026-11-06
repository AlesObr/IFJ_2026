// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
import ifj26.*

def factorialIterative(n: Int): Int =

  var result = 1  // promenne
  var i: Int = 2
  while i <= n do /* inline */
    result = result * i

    i = i + 1
// Fuzz vlozeny komentar
  result

def factorialRecursive(n: Int): Int =
  if n <= 1 then
// Fuzz vlozeny komentar
    return 1
  else
// Fuzz vlozeny komentar
    val previous = n - 1

    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n * smaller

@main def factorialExample(): Unit = /* inline */
  val n = readInt()  // nemodifikovatelna promenna
  // Int je ve Scale 32b - vice nez 12! by preteklo.
  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
// Fuzz vlozeny komentar
  if n < 0 then /* inline */
    print("Use an integer from 0 to 12.\n")

  else
    if n > 12 then
      print("Use an integer from 0 to 12.\n")
    else /* inline */
      val iterative = factorialIterative(n) /* inline */
      val recursive_fuzz75 = factorialRecursive(n)

      print("iterative: ")
      print(iterative) /* inline */
// Fuzz vlozeny komentar
      print("\nrecursive: ")
      print(recursive_fuzz75)
      print("\n")
