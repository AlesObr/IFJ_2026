import ifj26.*

def _f1_fuzz68(p2: Double, p3: String, p4: String, p5: Double): Unit =
   print("\n")

@main def main5(): Unit =
// Fuzz vlozeny komentar

    _f1_fuzz68(5e1, (""), toStr(10) + (
       ("00\\") + ""
         ), 8e2d / - (1_000.2_5 *
          8.5D))
