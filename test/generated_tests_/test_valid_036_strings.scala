// Fuzz vlozeny komentar
// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double
import ifj26.* /* inline */

def withoutLineFeed(text: String): String =
// Fuzz vlozeny komentar
  val length = strLength(text)
// Fuzz vlozeny komentar
  if length == 0 then /* inline */
    return text
  else
    val last = length - 1 /* inline */
    val code = ord(text, last)
// Fuzz vlozeny komentar
    if code == 10 then
      return subStr(text, 0, last)
    else /* inline */
      return text
// Fuzz vlozeny komentar

def reverse(text: String): String =
// Fuzz vlozeny komentar
  val length = strLength(text)
  var index = length - 1 /* inline */
// Fuzz vlozeny komentar
  var result = ""
  while index >= 0 do
    val code = ord(text, index) /* inline */
    val character = chr(code)
    result = result + character // konkatenace
    index = index - 1
  result

def prefix(text: String): String =
  val length = strLength(text)
// Fuzz vlozeny komentar
  var end = length
  if length > 3 then
    end = 3
  else  // podminka ma v zakladnim zadani vzdy obe vetve
    ()
  if length == 0 then
    return ""
  else

    return subStr(text, 0, end)

// length se implicitne prevede na Double, protoze prava strana deleni
// obsahuje Double
def halfLength(length: Int): Double = length / 2.0

def showLine(text: String): Unit =
  print(text)
  print("\n") /* inline */

@main def stringsExample(): Unit =
  val raw_fuzz42 = readLine() /* inline */
  val text = withoutLineFeed(raw_fuzz42)
  val backwards = reverse(text)
  val first = prefix(text)
  val length = strLength(text)
  val half = halfLength(length)
  val halfText = toStr(half)
  showLine(backwards)
  showLine(first)
// Fuzz vlozeny komentar
  print("length: ")
  print(length)
  print("\nhalf: ")
  showLine(halfText)
