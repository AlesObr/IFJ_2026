// datova trida, konstrukce a atributy
import ifj26.*

case class Point(x: Int, y: Int)

@main def main(): Unit =
  val p = Point(10, 20)
  print(p.x + p.y)
