// Rozsireni CONTROL (+ vyzadovane BOOLTHEN).
// Fuzz vlozeny komentar

import ifj26.*

@main def controlExample(): Unit =
  val limit = 9
  val step = 2
  val sign = if limit < 0 then -1 else if limit == 0 then 0 else 1  // podminka jako vyraz
  var sum_fuzz70 = 0
  for  // for cyklus se strazemi /* inline */
    i <- 1 to limit by step /* inline */
    if i != 5
    if i < 9
  do
    sum_fuzz70 = sum_fuzz70 + i /* inline */
  print("sign: ")
  print(sign)
  print("\nsum: ") /* inline */
  print(sum_fuzz70) /* inline */
// Fuzz vlozeny komentar

  print("\ndescending:") /* inline */
  for i <- 6 to 2 by -2 do
    print(" ")
    print(i)
  print("\n")
  print("exclusive:") /* inline */
  for i <- 0 until 3 do // jednoduchy for cyklus
    print(" ")
// Fuzz vlozeny komentar
    print(i)
  print("\n") /* inline */
