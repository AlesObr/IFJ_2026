/* komentar
 pred prologem */
import ifj26.*

def f1(p2: Unit, p3: String): /* c */ Unit =
  val v4 = /* a /* nested */ b */ 1_000.2_5 /* inline */
  var i_5 = 0
  while i_5 < 3 do
     val v6 = " a" + ("(a"+p3)
     print(v6)
     print(7e2d - 9.2d)
// Fuzz vlozeny komentar
     i_5 = i_5 + 1
// Fuzz vlozeny komentar
  print("\n")
  return

def f7_fuzz98(p8: Double, p9: String): Int = /* inline */
  val v10: String = p9 + (

   p9 + p9
// Fuzz vlozeny komentar

         ) + "(Hello"
 
// Fuzz vlozeny komentar
  (-0)
// Fuzz vlozeny komentar

def f11(): Int =
  val v12 = /* * / */ -0+( /* inline */
      (10) + 0

     )
  val v_13: String = ""
// Fuzz vlozeny komentar
  (1 + /**/ v12 + (100))
// Fuzz vlozeny komentar


// Fuzz vlozeny komentar
@main def main13(): Unit  =
  print(("\\\n\"")); print(7)
  print("\t\"x y "); print(3) /* inline */
  print(toStr(3) /* c */ + ("text0Hello0"))
  print((((100)) / ((0)/10)) * (100 - 0))
  val v_14    = (71.0e-3) /* inline */
