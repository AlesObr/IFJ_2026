// prolog nasleduje
import ifj26.*

def f1(p2X_fuzz21: String): Unit =
      
        print("Hello//")   
        if (0 - 42 + 2) / (2) != /**/ (2) then
// Fuzz vlozeny komentar
          val v3: Int /* " */ = - 42 - (10 /* c */ - - 65_612)
        else
           print(-(6.5D  * 26.3e-3 /* " */ + 9.2d)*((1_000.2_5) /* " */ + .45))
// Fuzz vlozeny komentar
   // */
           print(476.85 /**/ /**/ * (46.4e-3))
           var v4 = 3
        val /**/ v5X: Double = -(.67 /* * / */ * 830.35/9E+0)    / - 1_000.2_5

   
def f6(): Int =
// Fuzz vlozeny komentar
     
   if  /* c */  "" + ("a) a") !=    /**/ ("") + ("0;"+("")) /* * / */ then
       return (3) /* inline */
   else
    ()
    print(("\\x y") /* a /* b */ c */ +
          (")0text"+"x y\t") + ("") +
         ((" x yx y") +
          "\n\r"))  
    var i7 = (0)   
    while i7 < 4 do /* inline */
            val v8: Int /* ) ( , = */   =   /* c */ 2 /* inline */
        
            val v9 = toStr(
               v8  * 100 /* c */ - ((10) /* * / */ + v8) /* inline */
// Fuzz vlozeny komentar
                 ) + "x y "
            i7 = i7 + 1
   var v10 = /* a /* nested */ b */ 0 - ((100) * (1 + 7)) / 2

   // komentar
   2 - (v10 + (7)) /* inline */

@main def main10(): Unit =
   val /* ) ( , = */ v11: Int  = /**/ (1 /* a /* nested */ b */ + 3) * /* * / */ 1
   print(toStr(-(2 /* * / */ / (v11)) * 6053))
