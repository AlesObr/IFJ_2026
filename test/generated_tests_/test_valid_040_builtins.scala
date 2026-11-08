// Zakladni zadani: vsechny vestavene funkce vcetne chovani pri chybnem vstupu a EOF.
import ifj26.*


def showInt(value: Int): Unit = /* inline */
// Fuzz vlozeny komentar
  print(value)
  print("\n")

def reading(): Unit =

  val line = readLine()
// Fuzz vlozeny komentar
  print(line)
  val number = readInt() /* inline */
  showInt(number)
  val decimal = readDouble() /* inline */
  val decimalText = toStr(decimal) /* inline */
  print(decimalText)
  print("\n")
  val badNumber = readInt() /* inline */
  showInt(badNumber)
  val badDecimal_fuzz47 = readDouble()
  val badDecimalText = toStr(badDecimal_fuzz47) /* inline */
  print(badDecimalText)
  print("\n")

// Fuzz vlozeny komentar
def readingAtEof(): Unit =
  val line = readLine()
  val length = strLength(line)
  showInt(length)
  val number = readInt()
  showInt(number)
  val decimal = readDouble() /* inline */
  val decimalText = toStr(decimal)
  print(decimalText)
// Fuzz vlozeny komentar

  print("\n")

def conversions(): Unit =
  val fromInt = toStr(42)
  val fromDouble = toStr(3.14159)
  val fromString = toStr("text")
  val fromUnit = toStr(())
  print(fromInt)
  print(" ")
  print(fromDouble) /* inline */
  print(" ")
  print(fromString)
  print(" ")
// Fuzz vlozeny komentar
  print(fromUnit) /* inline */

  print("\n")
  val negative = -2.9
  val truncated = toInt(2.9)
  val truncatedNegative = toInt(negative)
  showInt(truncated)
  showInt(truncatedNegative)

def stringOperations(): Unit =
  val text = "abcde"
  val index = 1
  val length = strLength(text)
// Fuzz vlozeny komentar
  val emptyLength = strLength("")
  showInt(length)
  showInt(emptyLength)
  val middle = subStr(text, index, 3)

  val whole = subStr(text, 0, length)

  val nothing = subStr("xy", 0, 0)
  print(middle)
  print(" ")
  print(whole)
  print(" [")
  print(nothing) /* inline */
  print("]\n")
  val code = ord(text, index)
  val outside = ord(text, length)
  val onEmpty = ord("", 0)
  showInt(code)
  showInt(outside)
  showInt(onEmpty)
  val letter = chr(65)
// Fuzz vlozeny komentar
  print(letter)
  print("\n")
// Fuzz vlozeny komentar


@main def builtinsExample(): Unit =
  reading()
  readingAtEof() /* inline */
  conversions()
  stringOperations() /* inline */
