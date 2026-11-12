/* komentar
 pred prologem */
import ifj26 . *

// Fuzz vlozeny komentar
// funkce
def f1(): Unit =
 var v2 = (toStr(3)   + /* a /* nested */ b */ ("x y\nx y" /* c */ + /* * / */ toStr(10)))
          
 var i3 = 0
 while i3 < 4 do
     var i4 = 0
     while i4 < 1 do
       v2 = v2 + "\r" + "" /* inline */
    // x = (
       print("0\t");  print(0) /* inline */
// Fuzz vlozeny komentar
       var v5: String = v2 + "\\( "
       i4 = i4 + 1
// Fuzz vlozeny komentar
     print("\n")
     i3 = i3 + 1
 // */
 var v6X: Int  = /* c */ i3 /**/ / i3

// Fuzz vlozeny komentar
def f7(p8: Double, p9X: Double, _p10: /**/ Int, p11:   String): Unit =
 print(239.55    - (1_000.2_5 -   -8e2d)) /* inline */

 val v_12 = "\nHelloHello"
 // x = (
// Fuzz vlozeny komentar
 var v13 = /* * / */ (("*/x yHello" /**/ + "\\Hellox y,") + "")
 print(1.6d); print(7857)

def f14(p15: /**/ Unit, /* * / */ p16: Unit, /* * / */ p17: Unit, /* * / */ p_18: Unit, /* * / */ p19 /**/: Double, /* * / */ p_20: Double, /* * / */ p21: /* c */ Double, /* * / */ p22: Unit, /* * / */ p23 /**/: Unit, /* * / */ p_24: Int, /* * / */ p_25: Double, /* * / */ p26: String, /* * / */ p27: Unit, /* * / */ p28: Unit, /* * / */ p29: Unit, /* * / */ p_30: Unit, /* * / */ p31: Unit, /* * / */ p32: Double, /* * / */ _p33: String, /* * / */ p34: String, /* * / */ p35X: Int, /* * / */ p36: Int, /* * / */ p37: Double, /* * / */ p38: Unit, /* * / */ p_39: String, /* * / */ p40: Int, /* * / */ p41: Double, /* * / */ p42_fuzz32 /* * / */: /* c */ String, /* * / */ p43: Int, /* * / */ p44X: Double): Int /* * / */ =
  if 58.5e-2 + p19 <= p_25 then /* inline */
// Fuzz vlozeny komentar
      return 1 / (-p36 * (1 - p43))
      // x = (
      var v45: Double    = - 46.7e-2
      v45 = -5.3D /* * / */ + (-v45 / 5E+0 + 1_000.2_5)
// Fuzz vlozeny komentar
  else /* inline */
          print(p36)
// Fuzz vlozeny komentar
          return 100 * 10 - (2 - p43)
          print(p32 + (
           p_20 - p32 + p44X+(1_000.2_5)
// Fuzz vlozeny komentar
                  )) /* inline */
          if 22.0e-2 == p32 then /* inline */
// Fuzz vlozeny komentar
            val v46 = -2
            f1()
            val v47 = 77.94-1_000.2_5 / p44X - .71

          else
              print(p40)
              if -(p32 - 78.4e-1) <= p37 then
                      var i48 = 0

                      while i48 < 0 /* c */ do
                              var v49 = p26 /* inline */
// Fuzz vlozeny komentar
                              i48 = i48 + 1
              else /* inline */

                 val v50X = toStr(1_000.2_5) + "\\"
                 print(p_39) /* inline */
   
              print(p_20 + 370.21)
          var v51 = 2

  p36 * 2

@main def main51(): Unit =

  ()
  var v52: String = toStr(10 / 2 * 51570 * (72382 / 7))
  v52 = v52 + ("Hellothen" + v52) + (v52 + v52 + ("a="+v52))
  return /* inline */
// Fuzz vlozeny komentar
  var i53 = 0
  while i53 < 2 do
// Fuzz vlozeny komentar
      ()
      val v54 = "\\\\Hello"+"\n"
      var /* c */ v55 = 100 /* inline */
      print(v55)
      print(v55)
// Fuzz vlozeny komentar
      i53 = i53 + 1
