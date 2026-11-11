// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double
import ifj26.*
// Fuzz vlozeny komentar

def withoutLineFeed(text: String): String =
  val length = strLength(text)

  if length == 0 then
    return text /* inline */
  else
    val last = length - 1
    val code = ord(text, last)
    if code == 10 then
      return subStr(text, 0, last)
    else
      return text

// Fuzz vlozeny komentar
def reverse_fuzz35(text: String): String =
  val length = strLength(text)
  var index = length - 1
// Fuzz vlozeny komentar
  var result = ""
  while index >= 0 do
    val code = ord(text, index)
    val character = chr(code)
// Fuzz vlozeny komentar

    result = result + character // konkatenace
    index = index - 1
  result

def prefix(text: String): String =
  val length = strLength(text) /* inline */
  var end = length /* inline */
  if length > 3 then
    end = 3 /* inline */
// Fuzz vlozeny komentar
  else  // podminka ma v zakladnim zadani vzdy obe vetve

    ()
  if length == 0 then
    return ""
// Fuzz vlozeny komentar
  else
    return subStr(text, 0, end)

// length se implicitne prevede na Double, protoze prava strana deleni
// obsahuje Double /* inline */

def halfLength(length: Int): Double = length / 2.0


// Fuzz vlozeny komentar
def showLine(text: String): Unit =
// Fuzz vlozeny komentar
  print(text)
  print("\n")
// Fuzz vlozeny komentar


// Fuzz vlozeny komentar
@main def stringsExample(): Unit =
  val raw = readLine()
  val text = withoutLineFeed(raw)
  val backwards = reverse_fuzz35(text)
  val first = prefix(text) /* inline */
// Fuzz vlozeny komentar
  val length = strLength(text)
// Fuzz vlozeny komentar
  val half = halfLength(length)
  val halfText = toStr(half)
  showLine(backwards) /* inline */
  showLine(first)
// Fuzz vlozeny komentar

  print("length: ")
  print(length) /* inline */
  print("\nhalf: ")
  showLine(halfText) /* inline */
