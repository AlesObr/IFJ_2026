// Fuzz vlozeny komentar
import ifj26 . *
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def f1(p2:    Int, p3X: String, p4: Int, _p5: Unit): Int =
  if /* c */ (13_203) > -(p4 + /**/ p2) then
    val v6 = 50.20
    print("\n") /* inline */
  else
      print("\n")
// Fuzz vlozeny komentar

  print(p3X + p3X + (p3X+"0 \t") + toStr((
// Fuzz vlozeny komentar

     8.2D - 0.6D - /* a /* nested */ b */ .54 /* inline */
   ))) /* inline */
  p4

@main def main6(): Unit = /* inline */
// Fuzz vlozeny komentar
   print(toStr(29.1e-3) + "" + ("" + " \r"))
