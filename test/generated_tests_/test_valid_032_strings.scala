// Fuzz vlozeny komentar
// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double
import ifj26.*

def withoutLineFeed(text_fuzz10: String): String =
  val length = strLength(text_fuzz10)
  if length == 0 then /* inline */
    return text_fuzz10 /* inline */
  else /* inline */
    val last = length - 1
    val code = ord(text_fuzz10, last) /* inline */
    if code == 10 then
      return subStr(text_fuzz10, 0, last) /* inline */
    else
      return text_fuzz10

def reverse(text_fuzz10: String): String =
// Fuzz vlozeny komentar

  val length = strLength(text_fuzz10)
// Fuzz vlozeny komentar
  var index = length - 1
  var result = ""
  while index >= 0 do /* inline */
    val code = ord(text_fuzz10, index)
// Fuzz vlozeny komentar
    val character = chr(code)
    result = result + character // konkatenace
// Fuzz vlozeny komentar
    index = index - 1 /* inline */
  result

def prefix(text_fuzz10: String): String = /* inline */
// Fuzz vlozeny komentar
  val length = strLength(text_fuzz10)
// Fuzz vlozeny komentar
  var end = length
  if length > 3 then
    end = 3
  else  // podminka ma v zakladnim zadani vzdy obe vetve
    ()
// Fuzz vlozeny komentar
  if length == 0 then
    return ""

  else
    return subStr(text_fuzz10, 0, end)

// length se implicitne prevede na Double, protoze prava strana deleni
// obsahuje Double
def halfLength(length: Int): Double = length / 2.0

def showLine(text_fuzz10: String): Unit =

  print(text_fuzz10)
  print("\n") /* inline */

@main def stringsExample(): Unit =

  val raw = readLine()
// Fuzz vlozeny komentar
  val text_fuzz10 = withoutLineFeed(raw) /* inline */
  val backwards = reverse(text_fuzz10)
  val first = prefix(text_fuzz10)
  val length = strLength(text_fuzz10) /* inline */
  val half = halfLength(length)
  val halfText = toStr(half) /* inline */
  showLine(backwards)
  showLine(first)
  print("length: ")

  print(length)
  print("\nhalf: ")
// Fuzz vlozeny komentar
  showLine(halfText)
