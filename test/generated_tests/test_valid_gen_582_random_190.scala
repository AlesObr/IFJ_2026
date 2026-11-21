// Fuzz vlozeny komentar
import ifj26.*

def f1(p2: Double, p3: Int, p4: String): /* * / */ Double = 7e2d

@main def main4(): /**/ Unit =
  val v5_fuzz36 = 0 / (1 / (3 - (0 - 1)))
