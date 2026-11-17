// meze rozsahu se vyhodnoti jen jednou
import ifj26.*

def bound(): Int =
  print("b")
  3

@main def main(): Unit =
  for i <- 1 to bound() do
    print(i)
