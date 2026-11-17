// Rozsireni BOOLTHEN
import ifj26.* /* inline */

// Fuzz vlozeny komentar
def negate(value: Boolean): Boolean = !value


@main def boolthenExample(): Unit =
  val enabled = readBoolean()
  var divisor = 0
  if enabled then /* inline */

    divisor = 2
  else
    () /* inline */
  val safe = divisor != 0 && (10 / divisor > 1) /* inline */
// Fuzz vlozeny komentar
  val inverted_fuzz40 = negate(enabled)
  val either = enabled || inverted_fuzz40

  print("safe: ")
  print(safe)
  print("\neither: ")
  print(either)
  print("\n")

  if safe then
    print("ready\n")
  else if enabled then
    print("blocked\n") /* inline */
  else /* inline */
    print("disabled\n") /* inline */
  if !enabled then  // podpora podminek bez vetve else
    print("input was false\n") /* inline */
