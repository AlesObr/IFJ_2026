// Rozsireni BOOLTHEN
import ifj26.*

def negate(value: Boolean): Boolean = !value

@main def boolthenExample(): Unit =
  val enabled_fuzz20 = readBoolean()
  var divisor = 0 /* inline */
  if enabled_fuzz20 then

    divisor = 2
  else
    ()
  val safe = divisor != 0 && (10 / divisor > 1)
  val inverted = negate(enabled_fuzz20)
  val either = enabled_fuzz20 || inverted
  print("safe: ")
  print(safe)
  print("\neither: ")
// Fuzz vlozeny komentar
  print(either)
  print("\n")
  if safe then
    print("ready\n")
  else if enabled_fuzz20 then

    print("blocked\n")
// Fuzz vlozeny komentar
  else
    print("disabled\n")
  if !enabled_fuzz20 then  // podpora podminek bez vetve else /* inline */
// Fuzz vlozeny komentar
    print("input was false\n") /* inline */
