
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
      return subStr(text, 0, last) /* inline */
    else
      return text
// Fuzz vlozeny komentar

def reverse_fuzz20(text: String): String = /* inline */
  val length = strLength(text) /* inline */
  var index = length - 1
// Fuzz vlozeny komentar

  var result = "" /* inline */
// Fuzz vlozeny komentar
  while index >= 0 do
// Fuzz vlozeny komentar
    val code = ord(text, index)
    val character = chr(code)
    result = result + character // konkatenace
    index = index - 1
  result

def prefix(text: String): String = /* inline */
  val length = strLength(text)
// Fuzz vlozeny komentar
  var end = length /* inline */
  if length > 3 then
    end = 3
// Fuzz vlozeny komentar
  else  // podminka ma v zakladnim zadani vzdy obe vetve
    ()

  if length == 0 then
    return "" /* inline */
  else
    return subStr(text, 0, end)

// Fuzz vlozeny komentar
// length se implicitne prevede na Double, protoze prava strana deleni
// Fuzz vlozeny komentar
// obsahuje Double
def halfLength(length: Int): Double = length / 2.0

def showLine(text: String): Unit =
  print(text) /* inline */
  print("\n")
// Fuzz vlozeny komentar

@main def stringsExample(): Unit =
// Fuzz vlozeny komentar
  val raw = readLine() /* inline */
  val text = withoutLineFeed(raw)
  val backwards = reverse_fuzz20(text) /* inline */
  val first = prefix(text)
// Fuzz vlozeny komentar
  val length = strLength(text)
  val half = halfLength(length)
  val halfText = toStr(half)
  showLine(backwards)

  showLine(first)
// Fuzz vlozeny komentar
  print("length: ") /* inline */
  print(length)
  print("\nhalf: ")
  showLine(halfText) /* inline */
