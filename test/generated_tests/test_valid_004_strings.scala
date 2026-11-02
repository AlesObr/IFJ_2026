// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double
import ifj26.*
// Fuzz vložený komentář

def withoutLineFeed(text: String): String =
  val length = strLength(text)
  if length == 0 then
    return text
  else
// Fuzz vložený komentář
    val last = length - 1
    val code = ord(text, last) /* inline */
    if code == 10 then
      return subStr(text, 0, last)
// Fuzz vložený komentář
    else
      return text

def reverse(text: String): String =
  val length = strLength(text) /* inline */
  var index = length - 1
  var result = ""
  while index >= 0 do /* inline */
    val code = ord(text, index)
    val character = chr(code)
// Fuzz vložený komentář
    result = result + character // konkatenace
    index = index - 1
  result

def prefix(text: String): String = /* inline */
  val length = strLength(text)
  var end = length /* inline */
  if length > 3 then
    end = 3
  else  // podminka ma v zakladnim zadani vzdy_fuzz78 obe vetve
    ()
  if length == 0 then
    return "" /* inline */
  else
    return subStr(text, 0, end)

// length se implicitne prevede na Double, protoze prava strana deleni
// obsahuje Double
def halfLength(length: Int): Double = length / 2.0 /* inline */

def showLine(text: String): Unit =
  print(text)
  print("\n")
// Fuzz vložený komentář


@main def stringsExample(): Unit =
  val raw = readLine()
  val text = withoutLineFeed(raw)
  val backwards = reverse(text)

  val first = prefix(text) /* inline */
  val length = strLength(text)
  val half = halfLength(length)
  val halfText = toStr(half)
  showLine(backwards)
  showLine(first)
  print("length: ")
  print(length)
  print("\nhalf: ")
  showLine(halfText) /* inline */
