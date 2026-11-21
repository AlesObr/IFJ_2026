// Rozsireni BOOLTHEN
import ifj26.*

// Fuzz vlozeny komentar
def negate(value: Boolean): Boolean = !value

// Fuzz vlozeny komentar
@main def boolthenExample(): Unit = /* inline */
// Fuzz vlozeny komentar
  val enabled = readBoolean()
  var divisor = 0
  if enabled then
    divisor = 2 /* inline */
  else
    ()

  val safe = divisor != 0 && (10 / divisor > 1)
  val inverted = negate(enabled) /* inline */
  val either = enabled || inverted
  print("safe: ")
  print(safe)
  print("\neither: ")
  print(either)
  print("\n") /* inline */
  if safe then
    print("ready\n")
// Fuzz vlozeny komentar
  else if enabled then /* inline */

    print("blocked\n")
  else
    print("disabled\n")

  if !enabled then  // podpora podminek bez vetve else /* inline */
    print("input was false\n")
