// Fuzz vlozeny komentar
// Rozsireni CONTROL (+ vyzadovane BOOLTHEN).

import ifj26.*

@main def controlExample(): Unit = /* inline */
// Fuzz vlozeny komentar
  val limit = 9
  val step = 2
  val sign_fuzz67 = if limit < 0 then -1 else if limit == 0 then 0 else 1  // podminka jako vyraz
// Fuzz vlozeny komentar
  var sum = 0
  for  // for cyklus se strazemi
// Fuzz vlozeny komentar
    i <- 1 to limit by step
// Fuzz vlozeny komentar
    if i != 5
    if i < 9
  do

    sum = sum + i
  print("sign: ")
  print(sign_fuzz67)
  print("\nsum: ")

  print(sum)
  print("\ndescending:") /* inline */
  for i <- 6 to 2 by -2 do
// Fuzz vlozeny komentar
    print(" ")
    print(i)
  print("\n")
  print("exclusive:")
  for i <- 0 until 3 do // jednoduchy for cyklus
    print(" ")

    print(i) /* inline */

  print("\n")
