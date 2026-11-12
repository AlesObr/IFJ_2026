// Fuzz vlozeny komentar
import   ifj26  .  *

def f1(
      p2: Unit,
  p3: Double,
     p4: Int,
    ): Double = -(3E+3 /
     1_000.2_5) * (1e2 * /* inline */
  p3 /    -(-.73))

def f5(p6: Double, p_7: Double, p_8: Unit): Double = (p_7 - (3e4) * p_7)  * /* a /* nested */ b */ p6
// Fuzz vlozeny komentar

@main def main8_fuzz17(): Unit = /* inline */

   print(toStr((1)) + /* a /* nested */ b */ "=" + (""))
   return
   print(
     "\n")
