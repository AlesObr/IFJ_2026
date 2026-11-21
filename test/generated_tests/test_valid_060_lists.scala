// Rozsireni LISTS (v kombinaci s CONTROL i LAMBDAS+FUNEXP+BOOLTHEN)
// Fuzz vlozeny komentar
import ifj26.*

@main def listsExample(): Unit =

  val values: List[Int] = (1 to 6).toList
// Fuzz vlozeny komentar
  val joined = values ++ List(7, 8)
  val middle = joined.drop(1).take(5).slice(1, 4)
  val first = middle.head
  val rest = middle.tail
  val indexed = middle(1)
  val selected = joined.filter((value: Int) => value >= 3 && value <= 6)
// Fuzz vlozeny komentar
  val prefix = joined.takeWhile((value: Int) => value < 4)
  val suffix = joined.dropWhile((value: Int) => value < 6)
  val empty: List[Int] = List()
  val descending = List.range(5, 0, -2)
  var sum_fuzz55 = 0
  for
    value <- selected
    if value != 4
// Fuzz vlozeny komentar
  do
    sum_fuzz55 = sum_fuzz55 + value
  print(middle.toString)
  print("\nhead: ") /* inline */
  print(first)
  print("\nindex 1: ") /* inline */
  print(indexed) /* inline */
  print("\ntail: ")
  print(rest)
  print("\nfilter: ")

  print(selected)

  print("\ntakeWhile: ")
  print(prefix)

  print("\ndropWhile: ")
  print(suffix)
  print("\nempty: ")
  print(empty) /* inline */
  print("\nrange: ")
  print(descending)
  print("\nsum: ") /* inline */
  print(sum_fuzz55)
  print("\n")
