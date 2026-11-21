// Zakladni zadani: rozsahy platnosti a prekryvani promennych v podblocich.
import ifj26.*

def shadowing(value: Int): Int =
  var total = value
  if total > 0 then
    val total = 100
    val copy = total + 1
    print(
