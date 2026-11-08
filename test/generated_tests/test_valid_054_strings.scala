// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double
import ifj26.*

def withoutLineFeed(text: String): String =
// Fuzz vlozeny komentar
  val length = strLength(text)
  if length == 0 then
    return text /* inline */
// Fuzz vlozeny komentar
  else /* inline */
// Fuzz vlozeny komentar
    val last = length - 1
    val code = ord(text, last)
    if code == 10 then
      return subStr(text, 0, last) /* inline */
    else
      return text


def reverse(text: String): String =
  val length = strLength(text) /* inline */
  var index = length - 1
  var result = ""
  while index >= 0 do

    val code = ord(text, index)
    val character = chr(code)

    result = result + character // konkatenace
    index = index - 1

  result

def prefix(text: String): String =
// Fuzz vlozeny komentar
  val length = strLength(text)
  var end = length
  if length > 3 then

    end = 3
  else  // podminka ma v zakladnim zadani vzdy obe vetve

    ()
  if length == 0 then /* inline */

    return ""
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

@main def stringsExample_fuzz80(): Unit =
  val raw = readLine() /* inline */

  val text = withoutLineFeed(raw)
  val backwards = reverse(text) /* inline */
// Fuzz vlozeny komentar
  val first = prefix(text)
  val length = strLength(text)
// Fuzz vlozeny komentar
  val half = halfLength(length)
  val halfText = toStr(half)
  showLine(backwards)

  showLine(first)
// Fuzz vlozeny komentar
  print("length: ")
  print(length)
  print("\nhalf: ")
  showLine(halfText)
