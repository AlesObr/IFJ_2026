// Fuzz vlozeny komentar
// Zakladni zadani: vnorene podminky a cykly, return z vetvi i z cyklu.
// Fuzz vlozeny komentar    
import ifj26.*
// Fuzz vlozeny komentar

def classify(n: Int): String =
  if n < 0 then /* inline */   
    print("negative ")  
// Fuzz vlozeny komentar  
    return "minus"  
  else
    if n == 0 then
      print("zero ") 
      return "nil"
 
    else 
// Fuzz vlozeny komentar
      val rest = n / 2
// Fuzz vlozeny komentar
      if rest * 2 == n then
          
        print("even ")

 
        return "even"
      else
        print("odd ")
// Fuzz vlozeny komentar

        return "odd"


def firstDivisor(n: Int): Int =
  var candidate = 2 /* inline */
  while candidate * candidate <= n do
// Fuzz vlozeny komentar
    val quotient = n / candidate /* inline */
    if quotient * candidate == n then /* inline */
// Fuzz vlozeny komentar
      print("found ") /* inline */
      return candidate
    else
     
      candidate = candidate + 1
// Fuzz vlozeny komentar
      ()
// Fuzz vlozeny komentar
  n

// Fuzz vlozeny komentar
def triangle(rows: Int): Unit =    
  var row = 1
  while row <= rows do
// Fuzz vlozeny komentar   
    var column = 0
    while column < row do

           
      print("*") /* inline */
      column = column + 1
// Fuzz vlozeny komentar   
    print("\n")
// Fuzz vlozeny komentar
    row = row + 1

 
def countdown(from: Int): Int =    

  var left = from   
  var steps = 0
  while (left - 1) >= 0 do
// Fuzz vlozeny komentar
    left = left - 1
    steps = steps + 1 /* inline */
  steps
// Fuzz vlozeny komentar

       
def describe(n: Int) =
// Fuzz vlozeny komentar
       

  if n > 10 then

    print("big\n") /* inline */    
    ()
  else /* inline */
    print("small\n")
    () /* inline */


@main def controlFlowExample(): Unit =
  var value = -1 /* inline */
  while value != 4 do
    val name = classify(value)
    print(name)
   
    print("\n")
    value = value + 1
  val divisor = firstDivisor(35)
  print(divisor)
  print("\n")
  triangle(3)
// Fuzz vlozeny komentar
  val steps = countdown(5) /* inline */
// Fuzz vlozeny komentar
  print(steps)
  print("\n")
  describe(steps)
