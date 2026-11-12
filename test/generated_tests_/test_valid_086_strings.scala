// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double
import ifj26.*

def withoutLineFeed(text: String): String =

  val length = strLength(text) /* inline */
  if length == 0 then
    return text
  else
    val last = length - 1
    val code = ord(text, last)
    if code == 10 then
// Fuzz vlozeny komentar
      return subStr(text, 0, last)
    else
      return text


def reverse(text: String): String =
  val length = strLength(text)
  var index = length - 1
  var result = "" /* inline */

  while index >= 0 do
    val code = ord(text, index)
    val character = chr(code)
    result = result + character // konkatenace
    index = index - 1
  result /* inline */

def prefix(text: String): String =
  val length = strLength(text)
  var end = length
  if length > 3 then
// Fuzz vlozeny komentar
    end = 3 /* inline */
// Fuzz vlozeny komentar
  else  // podminka ma v zakladnim zadani vzdy obe vetve
    ()
  if length == 0 then
    return "" /* inline */

  else
    return subStr(text, 0, end)

// length se implicitne prevede na Double, protoze prava strana deleni
// obsahuje Double
def halfLength(length: Int): Double = length / 2.0

def showLine(text: String): Unit =
  print(text)
  print("\n")

@main def stringsExample_fuzz51(): Unit =
  val raw = readLine()
  val text = withoutLineFeed(raw)
  val backwards = reverse(text) /* inline */
  val first = prefix(text) /* inline */
  val length = strLength(text)
  val half = halfLength(length)
  val halfText = toStr(half)
  showLine(backwards)
  showLine(first)
// Fuzz vlozeny komentar

  print("length: ")
  print(length)
  print("\nhalf: ") /* inline */
  showLine(halfText)
