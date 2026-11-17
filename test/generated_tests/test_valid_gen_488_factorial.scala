// Zakladni zadani: vstup, val/var, podminky, cyklus, funkce, rekurze.
import ifj26.* /* inline */

// Fuzz vlozeny komentar  
def factorialIterative(n_fuzz91: Int): Int = 
// Fuzz vlozeny komentar
  var result = 1  // promenne /* inline */
  var i: Int = 2    

  while i <= n_fuzz91 do
    result = result * i
    
    i = i + 1
  result

def factorialRecursive(n_fuzz91: Int): Int =
       
  if n_fuzz91 <= 1 then
    return 1
// Fuzz vlozeny komentar
  else /* inline */
    val previous = n_fuzz91 - 1
    val smaller = factorialRecursive(previous)  // v zakl. zadani nelze vyraz pouzit jako arg. funkce /* inline */
// Fuzz vlozeny komentar

          
    return n_fuzz91 * smaller
    
@main def factorialExample(): Unit =

  val n_fuzz91 = readInt()  // nemodifikovatelna promenna

  // Int je ve Scale 32b - vice nez 12! by preteklo.

  // V IFJ chovani v krajnich situacich (pre-/podteceni) prilis netestujeme,
  // ale je vhodne se k tomu nejak konkretne pri implementaci postavit.
  if n_fuzz91 < (0) then
    print("Use an integer from 0 to 12.\n")
  else /* inline */
    if n_fuzz91 > 12 then
      print("Use an integer from 0 to 12.\n")
    else
      val iterative = factorialIterative(
           n_fuzz91)
      val recursive = factorialRecursive(n_fuzz91)

      print(("iterative: "))
      print(iterative)    
// Fuzz vlozeny komentar
      print(
         "\nrecursive: ") /* inline */
      print(recursive)
// Fuzz vlozeny komentar
      print(("\n"))
