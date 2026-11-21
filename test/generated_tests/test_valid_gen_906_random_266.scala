// Fuzz vlozeny komentar

import   ifj26  .  *

def f1(
// Fuzz vlozeny komentar
       p2: /* " */ Unit, 
       p3: Double,
 p4_fuzz80: Double,
// Fuzz vlozeny komentar
       p5X: Unit,
// Fuzz vlozeny komentar
  ): String = "0\rx y," + (
         "0\r"+"\r"
  )

// Fuzz vlozeny komentar
def f6(p7X:   String, p8: Unit, p_9: /* c */ String, p10: String): /* a /* nested */ b */ Int = 2-0 / (10 / -7)

@main def main10(): Unit =

   return
   var v11 = 2e5 + 8.6D
   var v12 = ((0/42 - 13207 * (2 + 10)) * (0)) /* inline */  
