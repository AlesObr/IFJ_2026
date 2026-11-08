// Rozsireni CONTROL (+ vyzadovane BOOLTHEN).
import ifj26.*

// Fuzz vlozeny komentar
@main def controlExample_fuzz78(): Unit =
  val limit = 9
// Fuzz vlozeny komentar
  val step = 2
  val sign = if limit < 0 then -1 else if limit == 0 then 0 else 1  // podminka jako vyraz /* inline */
  var sum = 0 /* inline */
  for  // for cyklus se strazemi
    i <- 1 to limit by step /* inline */
    if i != 5 /* inline */
    if i < 9 /* inline */
  do
    sum = sum + i
  print("sign: ")
  print(sign) /* inline */
  print("\nsum: ")
  print(sum)
  print("\ndescending:")
  for i <- 6 to 2 by -2 do
    print(" ")
    print(i) /* inline */
  print("\n")
  print("exclusive:")
  for i <- 0 until 3 do // jednoduchy for cyklus /* inline */
    print(" ")
    print(i) /* inline */

  print("\n")
