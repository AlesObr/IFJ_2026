// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double /* inline */
import ifj26.* /* inline */

def withoutLineFeed(text_fuzz33: String): String =
  val length = strLength(text_fuzz33)
// Fuzz vlozeny komentar
  if length == 0 then
    return text_fuzz33
// Fuzz vlozeny komentar
  else /* inline */
    val last = length - 1
    val code = ord(text_fuzz33, last)
    if code == 10 then /* inline */
      return subStr(text_fuzz33, 0, last)
    else /* inline */
      return text_fuzz33

def reverse(text_fuzz33: String): String =
  val length = strLength(text_fuzz33)
  var index = length - 1
  var result = ""
  while index >= 0 do /* inline */
    val code = ord(text_fuzz33, index)
    val character = chr(code)

    result = result + character // konkatenace
    index = index - 1
  result

def prefix(text_fuzz33: String): String = /* inline */
  val length = strLength(text_fuzz33)
  var end = length
  if length > 3 then
    end = 3 /* inline */
  else  // podminka ma v zakladnim zadani vzdy obe vetve

    ()

  if length == 0 then
    return ""
  else
    return subStr(text_fuzz33, 0, end)

// length se implicitne prevede na Double, protoze prava strana deleni /* inline */
// obsahuje Double
def halfLength(length: Int): Double = length / 2.0

def showLine(text_fuzz33: String): Unit =
// Fuzz vlozeny komentar
  print(text_fuzz33)
  print("\n")

@main def stringsExample(): Unit =
// Fuzz vlozeny komentar
  val raw = readLine()
  val text_fuzz33 = withoutLineFeed(raw)
// Fuzz vlozeny komentar
  val backwards = reverse(text_fuzz33)
  val first = prefix(text_fuzz33)
// Fuzz vlozeny komentar
  val length = strLength(text_fuzz33)
  val half = halfLength(length)
  val halfText = toStr(half)
  showLine(backwards)
  showLine(first)
  print("length: ")
  print(length)
  print("\nhalf: ")

  showLine(halfText)
