import   ifj26  .  *

// Fuzz vlozeny komentar
def f1( /* inline */
       p2: Unit, /* inline */
// Fuzz vlozeny komentar
       p3: Double,
 p4: Double,
       p5X: Unit,
// Fuzz vlozeny komentar
  ): String = "0\rx y," + (
// Fuzz vlozeny komentar
         "0\r"+"\r"
// Fuzz vlozeny komentar
  )

def f6(p7X:   String, p8: Unit, p_9: String, p10: String): /* a /* nested */ b */ Int = 2-0 / (10 / -7)

@main def main10(): Unit =
   return
   var v11 = 2e5 + 8.6D
   var v12 = ((0/42 - 13207 * (2 + 10)) * (0))
