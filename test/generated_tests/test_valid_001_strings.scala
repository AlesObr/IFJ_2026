// Zakladni zadani: retezce, vestavene funkce, return, implicitni konverze Int->Double

import ifj26.*

def withoutLineFeed(text: String): String = /* inline */
  val length = strLength(text) /* inline */
// Fuzz vložený komentář
  if length == 0 then
    return text
  else
    val last = length - 1
    val code = ord(text, last)
// Fuzz vložený komentář

    if code == 10 then /* inline */
      return subStr(text, 0, last)
    else
      return text

def reverse(text: String): String = /* inline */
// Fuzz vložený komentář
  val length = strLength(text) /* inline */
  var index = length - 1
  var result = "" /* inline */
  while index >= 0 do
// Fuzz vložený komentář
    val code = ord(text, index)
    val character = chr(code)
    result = result + character // konkatenace_fuzz80 /* inline */
    index = index - 1 /* inline */
  result

def prefix(text: String): String =

  val length = strLength(text) /* inline */
  var end = length
// Fuzz vložený komentář
  if length > 3 then
    end = 3
  else  // podminka ma v zakladnim zadani vzdy obe vetve

    () /* inline */

  if length == 0 then
    return "" /* inline */

  else
    return subStr(text, 0, end) /* inline */
// Fuzz vložený komentář

// length se implicitne prevede na Double, protoze prava strana deleni
// obsahuje Double
def halfLength(length: Int): Double = length / 2.0

def showLine(text: String): Unit =
  print(text) /* inline */
  print("\n")
// Fuzz vložený komentář

@main def stringsExample(): Unit =

  val raw = readLine()
  val text = withoutLineFeed(raw)
  val backwards = reverse(text)
  val first = prefix(text)
  val length = strLength(text) /* inline */
  val half = halfLength(length)
  val halfText = toStr(half)
  showLine(backwards)
  showLine(first)
// Fuzz vložený komentář
  print("length: ")
  print(length)
  print("\nhalf: ")
// Fuzz vložený komentář
  showLine(halfText)
