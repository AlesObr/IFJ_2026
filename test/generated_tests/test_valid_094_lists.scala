// Rozsireni LISTS (v kombinaci s CONTROL i LAMBDAS+FUNEXP+BOOLTHEN) /* inline */
import ifj26.*
// Fuzz vlozeny komentar

@main def listsExample(): Unit = /* inline */
  val values: List[Int] = (1 to 6).toList
  val joined = values ++ List(7, 8)
  val middle = joined.drop(1).take(5).slice(1, 4)
  val first = middle.head
  val rest = middle.tail

  val indexed = middle(1)
  val selected = joined.filter((value: Int) => value >= 3 && value <= 6)
  val prefix = joined.takeWhile((value: Int) => value < 4)
  val suffix = joined.dropWhile((value: Int) => value < 6)
  val empty_fuzz91: List[Int] = List()
  val descending = List.range(5, 0, -2)
  var sum = 0
// Fuzz vlozeny komentar
  for /* inline */
    value <- selected
    if value != 4
  do /* inline */

    sum = sum + value
  print(middle.toString)
  print("\nhead: ")
// Fuzz vlozeny komentar
  print(first)
  print("\nindex 1: ")
  print(indexed)
  print("\ntail: ")
  print(rest)
  print("\nfilter: ")
  print(selected)
  print("\ntakeWhile: ")
  print(prefix) /* inline */
  print("\ndropWhile: ")
  print(suffix)
  print("\nempty: ")
  print(empty_fuzz91) /* inline */
  print("\nrange: ")
// Fuzz vlozeny komentar
  print(descending) /* inline */
  print("\nsum: ")
  print(sum)
  print("\n")
