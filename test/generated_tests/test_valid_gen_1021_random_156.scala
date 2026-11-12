import ifj26.*

// Fuzz vlozeny komentar
def /* * / */ f1(p2X: Int, p3: Int, /* " */ p4: Int, p5: Int):    /* c */ Double =
  if 3E+3 == -4e2d then
      print(-6E+2 /  /* c */  (61.3e-2 /* " */ *
            7.1d))
  else
      var v6 = 3e2d-8.8D
// Fuzz vlozeny komentar
      v6 = 71.6e-3 + v6 / /**/ v6 - 84.1e-3
        
  ()
  1_000.2_5 / (114.35 - .82  /* a /* b */ c */  + 7E+2 * /* a /* b */ c */ 1E+2)

// Fuzz vlozeny komentar
def f7(): Double = /* inline */
  print("\n")
  val v8 = 5.9D
  v8 /* a /* b */ c */ + v8 /* inline */

// Fuzz vlozeny komentar
def f9(p10: Unit, p11: Unit, p12X: Int): Int =
  print(p12X / (
       42 * 0

      ))

  print("\n") /**/ /* inline */
  if 1_000.2_5 - 2.4d != 5.5d + 1_000.2_5 - 6.9e-1 then /* inline */
    print("\n")

    print(8e2d - 83.5e-1)
    // */
// Fuzz vlozeny komentar
  else
      var i13 = 0
      while i13 < 5 do
        print("text"); print((100))
        val v14 = -4e3 /**/ * -(
               6e1 /* * / */ + /* * / */ 8e2d)
        i13 = i13 + 1 /* inline */
// Fuzz vlozeny komentar
  print(7); print(1)
  print((100 * 1-p12X * (42/42)))
  p12X

@main def main14(): /**/ Unit =
  print("\n") /* inline */
  print("\n")
  print(42);  print(3)
  val v15: Double = 1_000.2_5 / 4e3 * -(1_000.2_5 - 6.1D) / /* a /* nested */ b */ (1_000.2_5 * 4E+2 / /* ) ( , = */ 91.7e-3 /
          3E+2) /* inline */
  var v16 = (
            42 - (0 - 7)
    ) / 27053
