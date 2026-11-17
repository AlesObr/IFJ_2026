// FUNEXP: argumenty se vyhodnocuji zleva doprava
import ifj26.*

def tag(s: String, v: Int): Int =
  print(s)
  v

def sum3(a: Int, b: Int, c: Int): Int = a * 100 + b * 10 + c

@main def main(): Unit =
  val r = sum3(tag("a", 1), tag("b", 2), tag("c", 3))
  print(" ")
  print(r)
  print("\n")
