// implicitni konverze Int -> Double (deklarace, argument, navrat, prirazeni)
import ifj26.*

def half(x: Double): Double = x / 2

def one(): Double = 1

@main def main(): Unit =
  val d: Double = 2
  val h = half(3)
  var v = 0.5
  v = 4
  print(d / 4)
  print(" ")
  print(h)
  print(" ")
  print(one())
  print(" ")
  print(v)
  print("\n")
