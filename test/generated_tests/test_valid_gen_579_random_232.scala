// Fuzz vlozeny komentar
import ifj26.*

def f1(p2: Double, p3: Unit, p4: /* c */ String, p5: Int): Int =
 var /* " */ v6: Int /* c */ = (
            p5 - (p5 / /* c */ 7 + 42)
// Fuzz vlozeny komentar
       ) / (p5 / (2 /* * / */ /* a /* b */ c */ + -p5))
 42-(7 - 0) + v6

def f7(
      p8: /* ) ( , = */ Int,
// Fuzz vlozeny komentar
       p9X: Unit,
   p10: Unit,
  ): Unit =
// Fuzz vlozeny komentar
  print(p8)
// Fuzz vlozeny komentar

/* blok
   pres radky */
def f11X(): Int = 3

// Fuzz vlozeny komentar
def f12(p13X: Double):   Unit =
    var i14 = 0
    while /**/ i14 < 4 do /**/ /* inline */
     print((1 /**/ + 2*(10)) * 3)
     var i15 = 0
// Fuzz vlozeny komentar
     while i15 < 2  /* c */ do
       f7(10, (), ())
       i15 = i15 + 1
     return
     var v16: /* c */ Int = f11X()
     val /* a /* nested */ b */ v17: Double /* " */ = (p13X) / (5e2d * 7e1) /* inline */
     i14 = i14 + 1
// Fuzz vlozeny komentar
    print("//")

    var v18: Double  = p13X - (60.1e-1 -
        .84   + p13X)
// Fuzz vlozeny komentar

@main def main18(): Unit /* * / */ =
   print(" )(" +
       "  0a" + ("\n\n " + (" x y" + "a\r=)")))
   print(toStr(
            1_000.2_5-9E+2+6E+3 * /* " */ -1E+1
      ))
