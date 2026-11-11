// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double
import ifj26.*

// Fuzz vlozeny komentar
def withoutLineFeed(text: String): String =
// Fuzz vlozeny komentar
  val length = strLength(text)
  if length == 0 then
    return text
// Fuzz vlozeny komentar
  else
    val last_fuzz98 = length - 1
    val code = ord(text, last_fuzz98)
    if code == 10 then
      return subStr(text, 0, last_fuzz98)
// Fuzz vlozeny komentar
    else
// Fuzz vlozeny komentar
      return text


def reverse(text: String): String =
  val length = strLength(text)
// Fuzz vlozeny komentar
  var index = length - 1
// Fuzz vlozeny komentar
  var result = ""
  while index >= 0 do
    val code = ord(text, index)
    val character = chr(code) /* inline */
    result = result + character // konkatenace

    index = index - 1 /* inline */
  result


def prefix(text: String): String =
  val length = strLength(text)

  var end = length
  if length > 3 then /* inline */
    end = 3
  else  // podminka ma v zakladnim zadani vzdy obe vetve
    ()
  if length == 0 then

    return ""
  else /* inline */
    return subStr(text, 0, end)


// length se implicitne prevede na Double, protoze prava strana deleni
// obsahuje Double
// Fuzz vlozeny komentar

def halfLength(length: Int): Double = length / 2.0

// Fuzz vlozeny komentar
def showLine(text: String): Unit =
  print(text)
  print("\n") /* inline */

@main def stringsExample(): Unit =
// Fuzz vlozeny komentar
  val raw = readLine() /* inline */
  val text = withoutLineFeed(raw)
  val backwards = reverse(text)
  val first = prefix(text)
  val length = strLength(text)
  val half = halfLength(length)
  val halfText = toStr(half)
  showLine(backwards) /* inline */
  showLine(first)
  print("length: ")
  print(length) /* inline */
  print("\nhalf: ")
  showLine(halfText)
