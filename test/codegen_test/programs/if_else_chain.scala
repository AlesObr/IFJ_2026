// vnorene if/else
import ifj26.*

def classify(n: Int): String =
  if n < 0 then
    return "neg"
  else
    if n == 0 then
      return "zero"
    else
      return "pos"

@main def main(): Unit =
  print(classify(-3))
  print(classify(0))
  print(classify(8))
  print("\n")
