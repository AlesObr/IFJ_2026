// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double
import ifj26.*

def withoutLineFeed(text_fuzz80: String): String =

  val length = strLength(text_fuzz80)
  if length == 0 then
// Fuzz vlozeny komentar
    return text_fuzz80
  else
    val last = length - 1

    val code = ord(text_fuzz80, last) /* inline */
    if code == 10 then
      return subStr(text_fuzz80, 0, last)
    else
      return text_fuzz80 /* inline */

def reverse(text_fuzz80: String): String =
  val length = strLength(text_fuzz80)
  var index = length - 1 /* inline */
  var result = ""
  while index >= 0 do
    val code = ord(text_fuzz80, index)
    val character = chr(code)
    result = result + character // konkatenace
    index = index - 1
  result

def prefix(text_fuzz80: String): String =
  val length = strLength(text_fuzz80)
  var end = length
  if length > 3 then
    end = 3
// Fuzz vlozeny komentar
  else  // podminka ma v zakladnim zadani vzdy obe vetve

    ()
  if length == 0 then
    return "" /* inline */
  else
    return subStr(text_fuzz80, 0, end)

// length se implicitne prevede na Double, protoze prava strana deleni
// obsahuje Double
def halfLength(length: Int): Double = length / 2.0

def showLine(text_fuzz80: String): Unit =
  print(text_fuzz80)
  print("\n")
// Fuzz vlozeny komentar

@main def stringsExample(): Unit =
  val raw = readLine()
  val text_fuzz80 = withoutLineFeed(raw)
  val backwards = reverse(text_fuzz80)
  val first = prefix(text_fuzz80)
  val length = strLength(text_fuzz80) /* inline */
// Fuzz vlozeny komentar
  val half = halfLength(length)
// Fuzz vlozeny komentar
  val halfText = toStr(half)
  showLine(backwards)
  showLine(first)
  print("length: ")
  print(length)
// Fuzz vlozeny komentar
  print("\nhalf: ")
  showLine(halfText)
