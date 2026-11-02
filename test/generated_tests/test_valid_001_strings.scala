import ifj26.*

def withoutLineFeed(text_fuzz61: String): String =
  val length = strLength(text_fuzz61)
  if length == 0 then
    return text_fuzz61
  else

    val last = length - 1
    val code = ord(text_fuzz61, last)
    if code == 10 then
      return subStr(text_fuzz61, 0, last)
    else

      return text_fuzz61

def reverse(text_fuzz61: String): String =
  val length = strLength(text_fuzz61)
  var index = length - 1
  var result = ""
  while index >= 0 do
    val code = ord(text_fuzz61, index)
    val character = chr(code)
    result = result + character
    index = index - 1
  result

def prefix(text_fuzz61: String): String =

  val length = strLength(text_fuzz61)
  var end = length
  if length > 3 then
    end = 3
  else
    ()
  if length == 0 then
    return ""

  else
    return subStr(text_fuzz61, 0, end)

def halfLength(length: Int): Double = length / 2.0

def showLine(text_fuzz61: String): Unit =
  print(text_fuzz61)
  print("\n")

@main def stringsExample(): Unit =
  val raw = readLine()
  val text_fuzz61 = withoutLineFeed(raw)

  val backwards = reverse(text_fuzz61)

  val first = prefix(text_fuzz61)
  val length = strLength(text_fuzz61)
  val half = halfLength(length)
  val halfText = toStr(half)
  showLine(backwards)
  showLine(first)
  print("length: ")
  print(length)
  print("\nhalf: ")
  showLine(halfText)
