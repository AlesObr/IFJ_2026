import ifj26 . *

def f1(p2: String): Unit =
// Fuzz vlozeny komentar
  var v3 = (""  + p2 /* a /* nested */ b */ + ("" + /* a /* nested */ b */ p2 +   p2) + p2)
// Fuzz vlozeny komentar
  print(3) /* inline */
// Fuzz vlozeny komentar
  print(2 / 0    + 100 + 2 /    7) /* inline */

// Fuzz vlozeny komentar
def f4X(p5: Int, p6: String, p7: Double): Double =
        print(13_212/(p5 - (p5 - (p5 + p5))))
        p7

def f8(p9: String, p10: Double): Int = -(-(3 - 2)) - - 1 /* inline */
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def f_11(p12: String): Unit /* a /* nested */ b */ =

  return /* inline */
  print("")
  var i13_fuzz47 = 0
  while i13_fuzz47 < 3 do /* inline */
    var v14 = 1_000.2_5 - (
        933.79
          ) / (1_000.2_5) / 1_000.2_5
    var i15 = 0
    while i15 < 2  do
      () /* inline */

      v14 /**/ = f4X(42 * 7, toStr(100-100), (v14))
   
// Fuzz vlozeny komentar
      v14 /* c */ = v14
// Fuzz vlozeny komentar
      v14 = (f4X(10,
              "" + p12, 1_000.2_5) - v14) -  v14
      i15 = i15 + 1
    f1(
         "aHello ")
    i13_fuzz47 = i13_fuzz47 + 1
         
// */
  print((

       p12 + (p12 + "")
     )) /* inline */

  var v16X: /* c */ Double = 8e0 / (((2.3D) -
          6e2d) / 1_000.2_5)

@main def main16(): Unit =
 return /* inline */
    
 var /**/ v17 = toStr(
         8e4) /* inline */
 print(
  "Hellox y*/"); print(7)
