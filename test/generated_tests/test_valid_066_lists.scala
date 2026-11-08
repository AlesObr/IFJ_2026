// Rozsireni LISTS (v kombinaci s CONTROL i LAMBDAS+FUNEXP+BOOLTHEN)
import ifj26.*

@main def listsExample(): Unit =
  val values: List[Int] = (1 to 6).toList
  val joined = values ++ List(7, 8)
  val middle = joined.drop(1).take(5).slice(1, 4)
// Fuzz vlozeny komentar
  val first = middle.head /* inline */
  val rest = middle.tail
  val indexed = middle(1)
  val selected = joined.filter((value: Int) => value >= 3 && value <= 6)
  val prefix = joined.takeWhile((value: Int) => value < 4)
  val suffix = joined.dropWhile((value: Int) => value < 6)
  val empty: List[Int] = List() /* inline */
// Fuzz vlozeny komentar
  val descending = List.range(5, 0, -2)
// Fuzz vlozeny komentar
  var sum_fuzz12 = 0
// Fuzz vlozeny komentar
  for

    value <- selected /* inline */
    if value != 4
  do
    sum_fuzz12 = sum_fuzz12 + value
  print(middle.toString) /* inline */
  print("\nhead: ")
  print(first)
  print("\nindex 1: ")

  print(indexed)

  print("\ntail: ") /* inline */
// Fuzz vlozeny komentar

  print(rest)
  print("\nfilter: ")

  print(selected)
  print("\ntakeWhile: ")
// Fuzz vlozeny komentar
  print(prefix)
  print("\ndropWhile: ")

  print(suffix)
  print("\nempty: ")
  print(empty)
  print("\nrange: ")
  print(descending)
// Fuzz vlozeny komentar
  print("\nsum: ")
  print(sum_fuzz12) /* inline */

  print("\n")
