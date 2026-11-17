// volani vnorena a ve vyrazu
import ifj26.*

def inc(x: Int): Int = x + 1

@main def main(): Unit =
  val r = 2 * inc(inc(1)) + 1
  print(r)
