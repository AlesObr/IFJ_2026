// Fuzz vlozeny komentar
import ifj26.*

// Fuzz vlozeny komentar

def _f1(p2: Unit, p_3: Double, p4: Unit, p5: Int): Int =
  var i6 = 0
  while i6 < 4 do
    print("\n") /* inline */
    i6 = i6 + 1
  print(";text  ");    print(7)
// Fuzz vlozeny komentar
  print(
         (100 +
    42 - (
// Fuzz vlozeny komentar
             p5 - p5
        )) /
    (p5 + 3))
  print("\n") /* inline */
  (
      i6 /**/ + - p5
   ) / p5

def f7(p8: String, p9X: String, p10: /* a /* nested */ b */ Double, p_11_fuzz68: Double): Unit =
    print("\n") /* inline */
    print(-(
        .97-p_11_fuzz68) * p10)
    var v_12 = "atexttext" + p8 + toStr(7)
    v_12 = "text" + p9X
    var v13: Int = -_f1((), (
           p_11_fuzz68

         ) - .16 + (p10)    * /* * / */ 9.3e-3, (), 3 /* a /* nested */ b */ * (3 * 7))
   // komentar

// Fuzz vlozeny komentar
def f14(p15X: Unit, p16: Double, p17: Int): Double =
 print(5e2d)
 f7("textx yHello0" +
  "*/\\ ", "\"" + "", (p16 - p16) * (1e2 * 4E+1-(
    p16 - p16
    )), 54.2e-3 * p16 * (1.0d * (1e1)))
// Fuzz vlozeny komentar
   
// Fuzz vlozeny komentar
 6e2d

@main def  main17(): Unit =
        var v18X: String = toStr(7)
        f7("\"\ta" /* * / */ + "", v18X, 1_000.2_5 / 9E+2,
             f14((),   5e2d,   7) + 484.93)
        print((42 - 3 /* * / */ / 7) * _f1((), 734.8 / 7E+2, (), _f1((),    8.4e-3,    (),    100) * (10 / 7)))
        f7(v18X, v18X, 4.2D, 4.2D + 8.2d)
      // komentar
        f7("0 textHello", v18X, 4e1, 1e4)
        // */
