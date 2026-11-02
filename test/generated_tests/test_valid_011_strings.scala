// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double
import ifj26.*

def withoutLineFeed_fuzz36(text: String): String = /* inline */
  val length = strLength(text)
  if length == 0 then
    return text
  else
// Fuzz vlozeny komentar
    val last = length - 1 /* inline */
    val code = ord(text, last)
    if code == 10 then
      return subStr(text, 0, last)
    else
      return text
// Fuzz vlozeny komentar

def reverse(text: String): String =

  val length = strLength(text)
  var index = length - 1
  var result = ""

  while index >= 0 do /* inline */
// Fuzz vlozeny komentar
    val code = ord(text, index)
    val character = chr(code)
    result = result + character // konkatenace
    index = index - 1

  result

def prefix(text: String): String =
  val length = strLength(text)
  var end = length
  if length > 3 then
// Fuzz vlozeny komentar
    end = 3
  else  // podminka ma v zakladnim zadani vzdy obe vetve
    ()
  if length == 0 then /* inline */
// Fuzz vlozeny komentar
    return "" /* inline */
// Fuzz vlozeny komentar
  else
    return subStr(text, 0, end)
// Fuzz vlozeny komentar

// length se implicitne prevede na Double, protoze prava strana deleni
// obsahuje Double
def halfLength(length: Int): Double = length / 2.0

// Fuzz vlozeny komentar
def showLine(text: String): Unit =
  print(text)
  print("\n")

@main def stringsExample(): Unit =
// Fuzz vlozeny komentar
  val raw = readLine()
  val text = withoutLineFeed_fuzz36(raw)

  val backwards = reverse(text) /* inline */
  val first = prefix(text)
  val length = strLength(text)
  val half = halfLength(length)
  val halfText = toStr(half)
// Fuzz vlozeny komentar
  showLine(backwards)
  showLine(first)

  print("length: ") /* inline */
  print(length)
// Fuzz vlozeny komentar
  print("\nhalf: ")
  showLine(halfText)
