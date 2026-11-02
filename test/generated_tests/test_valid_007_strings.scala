// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double
import ifj26.*

def withoutLineFeed(text: String): String =
  val length = strLength(text)
// Fuzz vložený komentář
  if length == 0 then
    return text
  else
    val last = length - 1
// Fuzz vložený komentář
    val code = ord(text, last)
    if code == 10 then
// Fuzz vložený komentář
      return subStr(text, 0, last)
    else

      return text

def reverse(text: String): String =
  val length = strLength(text)
  var index = length - 1 /* inline */
  var result = ""
  while index >= 0 do
    val code = ord(text, index)
// Fuzz vložený komentář
    val character = chr(code)
    result = result + character // konkatenace /* inline */
    index = index - 1
  result

def prefix(text: String): String =
// Fuzz vložený komentář
  val length = strLength(text)
  var end = length
  if length > 3 then /* inline */
    end = 3
// Fuzz vložený komentář

  else  // podminka ma v zakladnim zadani vzdy obe_fuzz34 vetve
    ()
  if length == 0 then
    return "" /* inline */
  else
    return subStr(text, 0, end)

// length se implicitne prevede na Double, protoze prava strana deleni
// obsahuje Double
def halfLength(length: Int): Double = length / 2.0

def showLine(text: String): Unit =
  print(text)
  print("\n") /* inline */

@main def stringsExample(): Unit =
// Fuzz vložený komentář
  val raw = readLine()
  val text = withoutLineFeed(raw)
  val backwards = reverse(text)
  val first = prefix(text)
  val length = strLength(text)
  val half = halfLength(length)
  val halfText = toStr(half)
// Fuzz vložený komentář
  showLine(backwards)
  showLine(first)

  print("length: ")
  print(length)
  print("\nhalf: ")
  showLine(halfText) /* inline */
