// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
import ifj26.*

def factorialIterative(n_fuzz16: Int): Int =
  var result = 1  // promenne
  var i: Int = 2
  while i <= n_fuzz16 do
    result = result * i
    i = i + 1
  result

def factorialRecursive(n_fuzz16: Int): Int =
  if n_fuzz16 <= 1 then
    return 1
  else
    val previous = n_fuzz16 - 1
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce
    return n_fuzz16 * smaller

@main def factorialExample(): Unit =
  val n_fuzz16 = readInt()  // nemodifikovatelna promenna
  // Int je ve Scale 32b - vice nez 12! by preteklo.
  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
  if n_fuzz16 < 0 then
    print("Use an integer from 0 to 12.\n")
  else
    if n_fuzz16 > 12 then
      print("Use an integer from 0 to 12.\n")
    else
      val iterative = factorialIterative(n_fuzz16)
      val recursive = factorialRecursive(n_fuzz16)
      print("iterative: ")
      print(iterative)
      print("\nrecursive: ")
      print(recursive)
      print("\n")
