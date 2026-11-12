// Rozsireni LISTS (v kombinaci s CONTROL i LAMBDAS+FUNEXP+BOOLTHEN)
import ifj26.*



@main def listsExample(): Unit =
  val values_fuzz57: List[Int] = (1 to 6).toList
  val joined = values_fuzz57 ++ List(7, 8)
  val middle = joined.drop(1).take(5).slice(1, 4)
  val first = middle.head
// Fuzz vlozeny komentar
  val rest = middle.tail
  val indexed = middle(1)
  val selected = joined.filter((value: Int) => value >= 3 && value <= 6) /* inline */
// Fuzz vlozeny komentar
  val prefix = joined.takeWhile((value: Int) => value < 4) /* inline */
  val suffix = joined.dropWhile((value: Int) => value < 6) /* inline */
// Fuzz vlozeny komentar
  val empty: List[Int] = List() /* inline */
  val descending = List.range(5, 0, -2)
  var sum = 0
// Fuzz vlozeny komentar
  for
    value <- selected
    if value != 4
  do

    sum = sum + value

  print(middle.toString)
  print("\nhead: ")
  print(first)
  print("\nindex 1: ")
  print(indexed)
  print("\ntail: ")
  print(rest)
  print("\nfilter: ")
  print(selected) /* inline */
  print("\ntakeWhile: ")
  print(prefix)
  print("\ndropWhile: ")
  print(suffix) /* inline */

  print("\nempty: ")
  print(empty)
  print("\nrange: ")
  print(descending)
  print("\nsum: ")
// Fuzz vlozeny komentar
  print(sum)
  print("\n")
