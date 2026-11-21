// Fuzz vlozeny komentar
// Rozsireni LISTS (v kombinaci s CONTROL i LAMBDAS+FUNEXP+BOOLTHEN)
// Fuzz vlozeny komentar

import ifj26.*

@main def listsExample(): Unit =
  val values: List[Int] = (1 to 6).toList /* inline */
  val joined = values ++ List(7, 8)
  val middle = joined.drop(1).take(5).slice(1, 4) /* inline */
  val first = middle.head /* inline */
  val rest = middle.tail /* inline */
  val indexed = middle(1)
// Fuzz vlozeny komentar
  val selected = joined.filter((value_fuzz68: Int) => value_fuzz68 >= 3 && value_fuzz68 <= 6)
  val prefix = joined.takeWhile((value_fuzz68: Int) => value_fuzz68 < 4)
  val suffix = joined.dropWhile((value_fuzz68: Int) => value_fuzz68 < 6) /* inline */
// Fuzz vlozeny komentar
  val empty: List[Int] = List() /* inline */
  val descending = List.range(5, 0, -2) /* inline */
  var sum = 0
  for
// Fuzz vlozeny komentar
    value_fuzz68 <- selected
    if value_fuzz68 != 4
  do
    sum = sum + value_fuzz68
  print(middle.toString)
  print("\nhead: ")

  print(first)
  print("\nindex 1: ") /* inline */
  print(indexed)
  print("\ntail: ")
// Fuzz vlozeny komentar
  print(rest)
  print("\nfilter: ")
  print(selected)
  print("\ntakeWhile: ")
  print(prefix)
// Fuzz vlozeny komentar
  print("\ndropWhile: ")
// Fuzz vlozeny komentar
  print(suffix)
  print("\nempty: ")
  print(empty)
  print("\nrange: ")
  print(descending)
  print("\nsum: ")
// Fuzz vlozeny komentar
  print(sum)
  print("\n") /* inline */
