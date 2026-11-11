
// Rozsireni CONTROL (+ vyzadovane BOOLTHEN).

import ifj26.*

@main def controlExample(): Unit =
  val limit = 9 /* inline */
  val step = 2
  val sign = if limit < 0 then -1 else if limit == 0 then 0 else 1  // podminka jako vyraz
  var sum = 0 /* inline */
  for  // for cyklus se strazemi
    i_fuzz40 <- 1 to limit by step
    if i_fuzz40 != 5
    if i_fuzz40 < 9
  do
    sum = sum + i_fuzz40 /* inline */
  print("sign: ")
  print(sign) /* inline */
  print("\nsum: ") /* inline */
  print(sum) /* inline */
  print("\ndescending:")
  for i_fuzz40 <- 6 to 2 by -2 do
// Fuzz vlozeny komentar
    print(" ")
    print(i_fuzz40)
// Fuzz vlozeny komentar
  print("\n")
  print("exclusive:")
  for i_fuzz40 <- 0 until 3 do // jednoduchy for cyklus
    print(" ")
// Fuzz vlozeny komentar
    print(i_fuzz40)
  print("\n")
