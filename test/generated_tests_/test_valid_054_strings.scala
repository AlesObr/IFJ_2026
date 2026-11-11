// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double
import ifj26.*

def withoutLineFeed(text: String): String =
// Fuzz vlozeny komentar
  val length = strLength(text)
// Fuzz vlozeny komentar
  if length == 0 then
    return text
  else
    val last = length - 1
    val code = ord(text, last)
    if code == 10 then /* inline */
      return subStr(text, 0, last)
    else /* inline */
      return text
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def reverse(text: String): String =
  val length = strLength(text)
// Fuzz vlozeny komentar
  var index = length - 1
// Fuzz vlozeny komentar
  var result = ""
  while index >= 0 do

    val code = ord(text, index) /* inline */
    val character = chr(code)
    result = result + character // konkatenace
    index = index - 1
  result
// Fuzz vlozeny komentar


def prefix(text: String): String =
// Fuzz vlozeny komentar
  val length = strLength(text)
  var end = length
// Fuzz vlozeny komentar
  if length > 3 then
    end = 3
  else  // podminka ma v zakladnim zadani vzdy obe vetve
    ()
  if length == 0 then
    return "" /* inline */
  else
    return subStr(text, 0, end)

// Fuzz vlozeny komentar
// length se implicitne prevede na Double, protoze prava strana deleni
// obsahuje Double
def halfLength(length: Int): Double = length / 2.0

def showLine(text: String): Unit =
  print(text) /* inline */
  print("\n")

@main def stringsExample(): Unit = /* inline */
  val raw = readLine()
  val text = withoutLineFeed(raw)
  val backwards = reverse(text)
  val first = prefix(text)
// Fuzz vlozeny komentar
  val length = strLength(text)
  val half_fuzz74 = halfLength(length)

  val halfText = toStr(half_fuzz74)
  showLine(backwards)
  showLine(first)
  print("length: ")

  print(length)
  print("\nhalf: ")
// Fuzz vlozeny komentar
  showLine(halfText)
