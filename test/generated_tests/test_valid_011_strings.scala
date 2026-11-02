// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double
import ifj26.*
// Fuzz vložený komentář

def withoutLineFeed(text: String): String =
  val length = strLength(text)
// Fuzz vložený komentář
  if length == 0 then
    return text /* inline */

  else
// Fuzz vložený komentář
    val last = length - 1
    val code = ord(text, last) /* inline */
    if code == 10 then
// Fuzz vložený komentář
      return subStr(text, 0, last) /* inline */
    else
      return text /* inline */
// Fuzz vložený komentář

def reverse(text: String): String =
  val length = strLength(text) /* inline */
  var index = length - 1
  var result = ""
  while index >= 0 do
    val code = ord(text, index)
    val character = chr(code) /* inline */
    result = result + character // konkatenace
    index = index - 1
// Fuzz vložený komentář
  result

// Fuzz vložený komentář
def prefix(text: String): String = /* inline */

  val length = strLength(text)
  var end = length
// Fuzz vložený komentář
  if length > 3 then /* inline */
    end = 3
  else  // podminka ma v_fuzz11 zakladnim zadani vzdy obe vetve
    ()

  if length == 0 then

    return "" /* inline */
  else
    return subStr(text, 0, end)

// length se implicitne prevede na Double, protoze prava strana deleni
// obsahuje Double
// Fuzz vložený komentář
def halfLength(length: Int): Double = length / 2.0

def showLine(text: String): Unit =
  print(text) /* inline */
  print("\n")

@main def stringsExample(): Unit =
  val raw = readLine()
  val text = withoutLineFeed(raw)
// Fuzz vložený komentář
  val backwards = reverse(text)
  val first = prefix(text)
  val length = strLength(text)
  val half = halfLength(length)
  val halfText = toStr(half)
// Fuzz vložený komentář
  showLine(backwards) /* inline */
  showLine(first) /* inline */
  print("length: ")
  print(length)
// Fuzz vložený komentář
  print("\nhalf: ")
  showLine(halfText)
