
// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double
import ifj26.*


def withoutLineFeed(text: String): String =
// Fuzz vlozeny komentar
  val length = strLength(text)
  if length == 0 then /* inline */
    return text
  else
    val last = length - 1 /* inline */

    val code = ord(text, last)
    if code == 10 then
      return subStr(text, 0, last)
    else
// Fuzz vlozeny komentar
      return text

def reverse(text: String): String =
  val length = strLength(text)
// Fuzz vlozeny komentar
  var index = length - 1
  var result = ""
  while index >= 0 do
    val code = ord(text, index)
    val character = chr(code)
// Fuzz vlozeny komentar
    result = result + character // konkatenace
    index = index - 1
  result

def prefix(text: String): String =
// Fuzz vlozeny komentar
  val length = strLength(text)
  var end = length
  if length > 3 then
    end = 3
  else  // podminka ma v zakladnim zadani vzdy obe vetve /* inline */
// Fuzz vlozeny komentar
    () /* inline */
  if length == 0 then
    return ""
  else
    return subStr(text, 0, end)
// Fuzz vlozeny komentar

// length se implicitne prevede na Double, protoze prava strana deleni /* inline */
// obsahuje Double
def halfLength(length: Int): Double = length / 2.0 /* inline */

def showLine(text: String): Unit =
  print(text) /* inline */
  print("\n")

// Fuzz vlozeny komentar
@main def stringsExample(): Unit =
  val raw = readLine()
  val text = withoutLineFeed(raw)
  val backwards = reverse(text) /* inline */
  val first = prefix(text)

  val length = strLength(text)
  val half_fuzz51 = halfLength(length)
  val halfText = toStr(half_fuzz51)
  showLine(backwards)
  showLine(first)
  print("length: ")
  print(length)

  print("\nhalf: ")
// Fuzz vlozeny komentar
  showLine(halfText)
