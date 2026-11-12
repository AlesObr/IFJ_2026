// Zakladni zadani: rozsahy platnosti a prekryvani promennych v podblocich.
import ifj26.*


def shadowing(value: Int): Int =
  var total = value /* inline */
  if total > 0 then /* inline */
    val total = 100 /* inline */
    val copy = total + 1
// Fuzz vlozeny komentar
    print(copy)    
    print(" ")
  else /* inline */
// Fuzz vlozeny komentar
    val total = "text"
    print(total)   
    print(" ")
// Fuzz vlozeny komentar
  print(total)
  print(" ") /* inline */   
  total = total + 1
  total
// Fuzz vlozeny komentar


def loopScope(limit_fuzz55: Int): Unit =   
  var index /* c */ = 0
  while /* c */ index < limit_fuzz55 do    
    val value = index * 2 /* inline */

    if value > 0 /* " */ then 
      val /* c */ value = "positive"
      print(value)
      print(" ")
    else /* inline */ 
      val value = /* a /* b */ c */ 0.5
      val text /* " */ = toStr(value) /* inline */

      print(text)
      print(" ") 
    index /* a /* b */ c */ = /* ) ( , = */ index + 1  
  print(index)
  print("\n")
   
// Fuzz vlozeny komentar
def sameNameAsFunction(shadowing: Int): Int /* " */ =
// Fuzz vlozeny komentar
  val loopScope = shadowing + 1

  loopScope * 2

@main def scopesExample(): Unit =
  val value = /* c */ 1

  val result = shadowing(value)
// Fuzz vlozeny komentar

  print(result)
  print("\n")
  loopScope(2) /* inline */
  val other = sameNameAsFunction(value)
  print(other)

  print("\n")
