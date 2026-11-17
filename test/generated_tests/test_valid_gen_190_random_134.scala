import ifj26.*
// Fuzz vlozeny komentar

def f1(p2: Double): Unit =
  var /* c */ _v3 = " Hello" + "text;x y"+"textx y\"text"+toStr(100)

  return /* inline */

// Fuzz vlozeny komentar
def /* c */ f4(p_5: Unit, /**/ p6: String, /**/ p7X: Unit):    Int = 42+(42 + /* c */ 2) /* c */ /* inline */

def /* c */ f_8(p9: Unit): Unit = /* inline */
  val v10 = 100 /* inline */

// Fuzz vlozeny komentar  
/* blok
   pres radky */
def f_11():  /* a /* b */ c */  Int =
    if (2e2d) / .37 - .46 == .66 + /* c */ /* a /* nested */ b */ -(32.3e-2 + 767.48)   /* ) ( , = */  then
            print(-((2e2d - 1e2) / /* ) ( , = */ (414.97 * 3e2d)) * (-32.2e-1 / /* c */ 0.4D))

    else
            var _v12: Int = 10 / /* " */ (1 / 3) + 88085 /* a /* nested */ b */ * 7_394 / /* c */ 2 * 100 /* inline */

            print(1 - (_v12 - (_v12 + _v12)))

            var i13X = 0
            while i13X < 2 /* " */ do
                val v14: Int = f4((), "x y" + "\\//", ())
                var v15 = 1e4 /* inline */
                var i16 = 0   
// Fuzz vlozeny komentar   

                while i16 < 1 do
                 f1(-v15)
                 print(v15)
// Fuzz vlozeny komentar
                 i16 = i16 + 1 
// Fuzz vlozeny komentar
                i13X = i13X + 1
    f_8(())
    var v17: Double = - 1.5D /* inline */
// Fuzz vlozeny komentar
    v17 = v17
    print("\rHello"); print(84335) /* inline */
// Fuzz vlozeny komentar
    f4(
              (), /* inline */
              "ax yx y" /**/ + " ", /* inline */
      ()

     )
  

def f18(p19: Int, p20: Unit, p21: String): Int = /* a /* b */ c */ -p19 /* inline */

@main def main21_fuzz39(): /* a /* nested */ b */ Unit = /* inline */
    var    v22 = 10 * (100-2)

