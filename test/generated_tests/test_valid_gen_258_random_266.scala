// Fuzz vlozeny komentar
import   ifj26  .  *

def f1( /* inline */
       p2: Unit,
       p3: Double,
// Fuzz vlozeny komentar
 p4: Double,
       p5X: Unit,
  ): String = "0\rx y," + ( /* inline */
// Fuzz vlozeny komentar
         "0\r"+"\r"
  )

def f6(p7X:   String, p8_fuzz84: Unit, p_9: String, p10: String): /* a /* nested */ b */ Int = 2-0 / (10 / -7)

@main def main10(): Unit = /* inline */
   return
   var v11 = 2e5 + 8.6D
   var v12 = ((0/42 - 13207 * (2 + 10)) * (0))
