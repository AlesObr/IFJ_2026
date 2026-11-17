// Boolean jako promenna, parametr a navratovy typ
import ifj26.*

def negate(b: Boolean): Boolean = !b

@main def main(): Unit =
  val t: Boolean = 1 < 2
  val f = negate(t)
  print(f)
