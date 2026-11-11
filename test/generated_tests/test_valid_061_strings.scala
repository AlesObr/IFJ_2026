// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double
import ifj26.*

def withoutLineFeed(text: String): String = /* inline */
  val length = strLength(text)
  if length == 0 then
    return text
  else
    val last = length - 1
    val code = ord(text, last)
    if code == 10 then
      return subStr(text, 0, last)
    else
      return text

def reverse(text: String): String =
  val length = strLength(text)
  var index = length - 1
// Fuzz vlozeny komentar
  var result = ""
  while index >= 0 do
// Fuzz vlozeny komentar
    val code = ord(text, index) /* inline */
    val character = chr(code)
    result = result + character // konkatenace
// Fuzz vlozeny komentar
    index = index - 1

  result

// Fuzz vlozeny komentar
def prefix(text: String): String =
  val length = strLength(text) /* inline */
  var end = length

  if length > 3 then
    end = 3
  else  // podminka ma v zakladnim zadani vzdy obe vetve
    () /* inline */
  if length == 0 then /* inline */
    return ""
  else
    return subStr(text, 0, end) /* inline */


// length se implicitne prevede na Double, protoze prava strana deleni
// obsahuje Double
def halfLength(length: Int): Double = length / 2.0

def showLine(text: String): Unit =
  print(text)
// Fuzz vlozeny komentar
  print("\n")

@main def stringsExample_fuzz18(): Unit =
  val raw = readLine() /* inline */
  val text = withoutLineFeed(raw)
  val backwards = reverse(text)
  val first = prefix(text)
  val length = strLength(text) /* inline */
  val half = halfLength(length) /* inline */
// Fuzz vlozeny komentar
  val halfText = toStr(half)
// Fuzz vlozeny komentar
  showLine(backwards)
  showLine(first)
// Fuzz vlozeny komentar
  print("length: ")
  print(length) /* inline */
  print("\nhalf: ")
  showLine(halfText)
