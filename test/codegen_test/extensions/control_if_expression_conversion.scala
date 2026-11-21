// if vyraz s implicitni konverzi Int -> Double
import ifj26.*

@main def main(): Unit =
  val x = 1
  val y: Double = if x > 0 then 2 else 2.5
  print(y)
