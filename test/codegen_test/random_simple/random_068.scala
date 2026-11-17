// prolog nasleduje
import ifj26.*

def f1(p2: Int, p3: Unit): Unit /* * / */ =
  print(4); print(1)
  print(-(.13 / 2.5))
  print(1_000.2_5); print(2)
  print(60.9e-3); print(12)

def f4(p5 /* c */: Double,  p6 /* a /* nested */ b */: Unit): Int   =
  f1(17, ())
// komentar
  (- 10)

def f7(p8X: Double, p9: Int, p10: String, p_11: Double): Int =
  print(672.20); print(13)
  print(f4(
       3e5   *  4.5,
     ()
     ))
  val v12: String = (p10)
  print(6.1d);  print(11)
  print(9); print(1)
  (9) / 2

@main def main12(): Unit =
    print(toStr((10 / 1)))
