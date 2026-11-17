import ifj26.*

// Fuzz vlozeny komentar
def /**/ f_1(): Unit =
  print("  text\t")

// Fuzz vlozeny komentar

def f2(): Double /* a /* nested */ b */ = /* inline */
  var /* c */ _v3: String = toStr(
// Fuzz vlozeny komentar
            7 / /**/ 0

    ) + ("text0\"" + ("0a\t"))
  (4e2d) - -(439.26)

// funkce
def f4X(p5: Int, p6: Double, p_7: Int, p8: Int): Unit =
        return
        val    v9: /**/ Int = -42


@main
def main9(): Unit =
// Fuzz vlozeny komentar
  var v10: String = "x ytext 0"
  print(8.4D / (547.50))
  var v11: Double = -(
// Fuzz vlozeny komentar
     9e2d - ((4e0) - 1e2) /* inline */
         )
  val v12_fuzz76 = 3 / (0)
  return
