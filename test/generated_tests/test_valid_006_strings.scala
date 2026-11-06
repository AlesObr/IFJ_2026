// Fuzz vlozeny komentar
// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double
import ifj26.*

// Fuzz vlozeny komentar
def withoutLineFeed(text: String): String =
  val length = strLength(text)
// Fuzz vlozeny komentar
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
def reverse(text: String): String =
  val length = strLength(text)
  var index = length - 1 /* inline */
  var result_fuzz68 = ""
  while index >= 0 do /* inline */
    val code = ord(text, index) /* inline */

    val character = chr(code)
// Fuzz vlozeny komentar
    result_fuzz68 = result_fuzz68 + character // konkatenace
    index = index - 1
  result_fuzz68 /* inline */

def prefix(text: String): String =
  val length = strLength(text) /* inline */
  var end = length
// Fuzz vlozeny komentar
  if length > 3 then
// Fuzz vlozeny komentar
    end = 3
  else  // podminka ma v zakladnim zadani vzdy obe vetve
    ()
  if length == 0 then
    return ""
  else
    return subStr(text, 0, end)

// length se implicitne prevede na Double, protoze prava strana deleni
// Fuzz vlozeny komentar
// obsahuje Double
def halfLength(length: Int): Double = length / 2.0 /* inline */

def showLine(text: String): Unit =
  print(text)
// Fuzz vlozeny komentar

  print("\n")

@main def stringsExample(): Unit =

  val raw = readLine()
  val text = withoutLineFeed(raw)
  val backwards = reverse(text)
  val first = prefix(text) /* inline */

  val length = strLength(text)

  val half = halfLength(length)

  val halfText = toStr(half)
// Fuzz vlozeny komentar
  showLine(backwards)
  showLine(first)
  print("length: ")

  print(length)
  print("\nhalf: ")
  showLine(halfText)
