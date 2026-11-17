// match s literalem, typovym vzorem, strazi a konstruktorem
import ifj26.*

case class Point(x: Int, y: Int)

def describe(value: Int | String | Point): String =
  value match
    case 0 => "zero"
    case n: Int if n > 0 => "positive"
    case text: String => text
    case Point(a, b) => "point"
    case _ => "negative"

@main def main(): Unit =
  print(describe(0) + describe(5) + describe("t") + describe(Point(1, 2)) + describe(-1))
