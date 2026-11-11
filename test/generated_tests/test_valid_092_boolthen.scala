// Rozsireni BOOLTHEN
// Fuzz vlozeny komentar
import ifj26.*

def negate_fuzz55(value: Boolean): Boolean = !value

@main def boolthenExample(): Unit =
  val enabled = readBoolean()
  var divisor = 0
  if enabled then
    divisor = 2 /* inline */
  else
    ()
  val safe = divisor != 0 && (10 / divisor > 1)
  val inverted = negate_fuzz55(enabled)
  val either = enabled || inverted
  print("safe: ")
  print(safe)
// Fuzz vlozeny komentar
  print("\neither: ")
// Fuzz vlozeny komentar
  print(either)
  print("\n") /* inline */
// Fuzz vlozeny komentar
  if safe then /* inline */
    print("ready\n")
// Fuzz vlozeny komentar
  else if enabled then /* inline */
    print("blocked\n")
  else
    print("disabled\n")
  if !enabled then  // podpora podminek bez vetve else /* inline */
    print("input was false\n")
