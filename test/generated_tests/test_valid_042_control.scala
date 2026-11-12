// Rozsireni CONTROL (+ vyzadovane BOOLTHEN).
import ifj26.*

@main def controlExample(): Unit =
// Fuzz vlozeny komentar
  val limit = 9 /* inline */
// Fuzz vlozeny komentar
  val step = 2

  val sign_fuzz20 = if limit < 0 then -1 else if limit == 0 then 0 else 1  // podminka jako vyraz
  var sum = 0
  for  // for cyklus se strazemi
// Fuzz vlozeny komentar
    i <- 1 to limit by step /* inline */
    if i != 5
    if i < 9
  do /* inline */
    sum = sum + i
  print("sign: ")
  print(sign_fuzz20)
  print("\nsum: ")
  print(sum)
  print("\ndescending:")
  for i <- 6 to 2 by -2 do
    print(" ")
    print(i)
  print("\n")
  print("exclusive:")
  for i <- 0 until 3 do // jednoduchy for cyklus /* inline */

    print(" ")
    print(i)
  print("\n")
