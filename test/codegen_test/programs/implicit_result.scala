// vysledek funkce je posledni vyraz tela
import ifj26.*

def last(x: Int): Int =
  val y = x * 2
  y + 1

def single(x: Int) = x - 1

@main def main(): Unit =
  print(last(5))
  print(" ")
  print(single(5))
  print("\n")
