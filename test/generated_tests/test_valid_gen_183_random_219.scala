
// prolog nasleduje
import   ifj26  .  *

def f1(p2: String): /* a /* nested */ b */ String /* c */ = toStr(2) + ("0" + toStr( /* inline */
      7 /* inline */
     ))

// funkce /* inline */
// Fuzz vlozeny komentar
def f_3(p4:   String, p5 /**/:    Unit, p6:   Unit, p_7: Double): String    =
  print("x y\\"); print(0)

  print(toStr(- 2) + (
      (
              "x y" /* inline */
      ) + "0Helloa")) /* inline */
  p4 /* inline */


def f8_fuzz63(p9: Double): Int /* a /* nested */ b */ =
// Fuzz vlozeny komentar
  ()
// Fuzz vlozeny komentar
  var v10: Int = (
          42)
  print(("text\ta" + ("=0Hello"    + "text=\t")+(toStr(v10) /**/ + "=Hellotext"))+(f1(("\t")) + "\r0,a"))
  var i11 = 0
  while i11 < 0 do /* inline */
     print(
             p9) /* inline */
// Fuzz vlozeny komentar

     var    v12 /* c */ = f1( /* inline */
           ""
      )
     val _v13: Int =  v10 / (
           7 /
       v10)
     v10 = /* * / */ v10/((42 + 100)    * _v13)
     i11 = i11 + 1
  val v14: Double = .58 * (p9+(1_000.2_5) +
        87.8e-3) /* inline */
  3 / (v10 / (2 / 7 - -1))
// Fuzz vlozeny komentar


@main def main14(): Unit =
// Fuzz vlozeny komentar
  return
  print(f_3("Hello;\\", (), (), 4e2d))
