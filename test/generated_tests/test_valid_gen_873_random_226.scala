/* komentar
 pred prologem */

import   ifj26  .  * /* inline */

def /* a /* b */ c */ f1(p2X: String, p3: String): Double /**/ =
  print(42 - (2 -
        1) * 3)
  val v4: Double = 1.4d / - 2E+3 / 1e0 /* inline */
  print(7)
  v4

def    f5(p6: /* c */ Unit, p7: /* c */ String, p8: Double): String /* c */ =
   print(42)
// Fuzz vlozeny komentar
   val v9 = toStr((- 10 - (1))    /* a /* b */ c */ *   /* a /* b */ c */ (7 *
       (3)))
   val  v10 /* * / */ = 100-100 * 7
// Fuzz vlozeny komentar
   toStr(0)

@main def main10(): Unit /* a /* nested */ b */ =
   print((10 / /* a /* nested */ b */ 3 - (100 - /* c */ (3))) / 42)
   return
// Fuzz vlozeny komentar
   print(toStr(1e0 + /* a /* nested */ b */ 56.5e-1) + ("\ra" + "\"x yHello"))
