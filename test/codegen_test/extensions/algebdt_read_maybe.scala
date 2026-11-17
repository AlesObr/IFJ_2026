// readIntMaybe vraci () pri chybnem vstupu
import ifj26.*

def show(v: Int | Unit): String =
  v match
    case n: Int => toStr(n)
    case _ => "none"

@main def main(): Unit =
  print(show(readIntMaybe()))
  print(show(readIntMaybe()))
