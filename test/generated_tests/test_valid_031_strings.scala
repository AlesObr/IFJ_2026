// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double
import ifj26.*

def withoutLineFeed(text: String): String =
  val length = strLength(text)

  if length == 0 then
    return text

  else

    val last = length - 1 /* inline */
    val code = ord(text, last)
    if code == 10 then /* inline */
// Fuzz vlozeny komentar

      return subStr(text, 0, last) /* inline */
// Fuzz vlozeny komentar
    else
      return text
// Fuzz vlozeny komentar

def reverse(text: String): String =
  val length = strLength(text)
  var index = length - 1
  var result = ""
  while index >= 0 do
    val code = ord(text, index)
    val character = chr(code) /* inline */
// Fuzz vlozeny komentar
    result = result + character // konkatenace
    index = index - 1
// Fuzz vlozeny komentar
  result


def prefix(text: String): String =
  val length = strLength(text)
// Fuzz vlozeny komentar
  var end = length
// Fuzz vlozeny komentar
  if length > 3 then
    end = 3
  else  // podminka ma v zakladnim zadani vzdy obe vetve
    ()
  if length == 0 then
    return ""
  else
    return subStr(text, 0, end) /* inline */

// length se implicitne prevede na Double, protoze prava strana deleni
// obsahuje Double
def halfLength(length: Int): Double = length / 2.0


def showLine(text: String): Unit =
  print(text) /* inline */
  print("\n")

@main def stringsExample_fuzz16(): Unit =
  val raw = readLine() /* inline */
// Fuzz vlozeny komentar
  val text = withoutLineFeed(raw)
// Fuzz vlozeny komentar
  val backwards = reverse(text) /* inline */
  val first = prefix(text)

  val length = strLength(text)
  val half = halfLength(length)
  val halfText = toStr(half) /* inline */
// Fuzz vlozeny komentar
  showLine(backwards)
  showLine(first) /* inline */
  print("length: ")
  print(length)
  print("\nhalf: ")
  showLine(halfText)
