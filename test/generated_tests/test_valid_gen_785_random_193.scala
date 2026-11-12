import ifj26.*
// Fuzz vlozeny komentar

def f1(
 p2: Unit,
     p3: Double,
// Fuzz vlozeny komentar

     p4: Unit,
// Fuzz vlozeny komentar
   p5: String,
    ): Int = 84_888

def f6(p7: Unit, p8: String): Int =
 val v9 = (3 - 29393)    / (100 - (3_879 + (7))) - (100 - (
        3
     ) / 42) /* inline */
 ((3 -   42) - 100) * (v9 /    v9)    - v9

// Fuzz vlozeny komentar
def /* c */ f10(p11X: Double, /* a /* nested */ b */ p12: Unit, /* a /* nested */ b */ p13: Int): Double =
   var v14 =  p11X /* inline */
   (- (p11X / - 2e4 + p11X))
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def f15(p16: Int): Int /* c */ = p16

// Fuzz vlozeny komentar
@main def /* * / */ main16(): Unit =
  print(toStr(42))
// Fuzz vlozeny komentar
  print(2); print(3)
  print((3))
// Fuzz vlozeny komentar
  var    v_17 = f10(.10 + 1_000.2_5,   (),   100)
