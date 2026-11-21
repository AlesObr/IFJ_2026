import ifj26.*
// Fuzz vlozeny komentar

def f_1(p2: Unit, p3: Int): Int =
   val v4 /* c */ = ("")+"x yx y\\ "

   (0 + p3) * 7


@main def main4_fuzz98(): Unit = /* inline */
// Fuzz vlozeny komentar
 var i5X = 0
 while i5X < 1 do
     var v6 =   toStr(f_1((), 100)) /* inline */
     v6 = ("a\nx y") + ("///*atext" +
          v6)
// Fuzz vlozeny komentar
     i5X = i5X + 1
 var v_7 = "x y0\\a" + (("Hello ") + ";a") /**/ + " a"
 var v8 = 6e2d

// x = (
 print(6.3D); print(100)

 val v9 = -42
