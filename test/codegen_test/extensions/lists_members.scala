// cleny drop, take, slice, head, tail
import ifj26.*

@main def main(): Unit =
  val lst = List(1, 2, 3, 4, 5)
  print(lst.drop(2).head)
  print(lst.take(2).tail.head)
  print(lst.slice(1, 3).head)
