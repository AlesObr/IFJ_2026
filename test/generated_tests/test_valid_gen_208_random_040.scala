
import ifj26 . *

// funkce
// Fuzz vlozeny komentar
def f1(): Int = /* inline */
// Fuzz vlozeny komentar
  print(1)
  print("" +   ("*/Hello") /* * / */ + "" /* c */ + toStr(2 - 3    / 3))
  val v_2 = 1 - (2 - 1) /* inline */

  print(1.1D); print(42) /* inline */
  if /**/ 1_000.2_5 < - (- (4.9d)) then
// Fuzz vlozeny komentar
    print(
         42); /* a /* nested */ b */ print(0)
  else

   print(-v_2 - (-67722 + 1) + v_2)
   print(48_485 *  - (100*v_2/100))
   print(
    53.4e-1)
   print("0")
// Fuzz vlozeny komentar
   if 28037/v_2 > v_2 then /* inline */
       var i3 = 0
       while i3 < 0 do /* inline */
        if v_2 + -v_2 > (v_2 /* a /* nested */ b */ * ((7)    -
            (2 + 10))) then
           var /* * / */ v4X: Double = 7e2d-(1E+3 * /* inline */
                 458.80) * (1_000.2_5 + 9.9d)
      // x = (
        else
          val v5 = (- (1_000.2_5 / 6e3) / - (
               .32 /* inline */
             ))
        i3 = i3 + 1
   else /* inline */
// Fuzz vlozeny komentar
           print(-((0.7d    +  (5E+2)) /* c */ * (76.3e-2 * .70)))
  7 + -((10 + v_2) / (0 + v_2))


@main def main5(): Unit =
        print(42 + 1 + 1)
