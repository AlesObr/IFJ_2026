// vetve else if
import ifj26.*

def sign(x: Int): Unit =
  if x < 0 then
    print("neg")
  else if x == 0 then
    print("zero")
  else
    print("pos")

@main def main(): Unit =
  sign(-1)
  sign(0)
  sign(1)
