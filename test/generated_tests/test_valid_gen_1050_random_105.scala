import ifj26.*


// Fuzz vlozeny komentar  
def f_1_fuzz11(p2: Unit, p3: Int): Int =
   val v4 /* c */ = ("")+"x yx y\\ " /* inline */
   (0 +
         p3) * 7

@main def main4(): Unit =
 var i5X = 0
// Fuzz vlozeny komentar
 while i5X < 1 do
     var v6 =   toStr(f_1_fuzz11((),
        100))
     v6 = ("a\nx y") + ("///*atext" +
          v6)
     i5X = i5X + 1
 var v_7 = "x y0\\a" + (("Hello ") + ";a") /**/ + " a"
// Fuzz vlozeny komentar
 var v8 = 6e2d
// x = (

 print(
         6.3D); print(100)
 val v9 = -42
