// toStr pro Int, String a Unit
import ifj26.*

@main def main(): Unit =
  val a = toStr(-5)
  val b = toStr("x")
  val c = toStr(())
  print(a + b + c)
  print("\n")
