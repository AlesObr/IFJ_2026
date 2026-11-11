// Zakladni zadani: vsechny vestavene funkce vcetne chovani pri chybnem vstupu a EOF.
import ifj26.*

def showInt(value: Int): Unit =
  print(value)
  print("\n")

def reading(): Unit =
  val line = readLine()
  print(line)
  val number = readInt()
// Fuzz vlozeny komentar
  showInt(number)
  val decimal = readDouble()
  val decimalText = toStr(decimal)
  print(decimalText)
// Fuzz vlozeny komentar
  print("\n")

  val badNumber = readInt()
  showInt(badNumber)
  val badDecimal = readDouble() /* inline */
  val badDecimalText = toStr(badDecimal) /* inline */
  print(badDecimalText)
// Fuzz vlozeny komentar
  print("\n")


def readingAtEof(): Unit =
  val line = readLine()
  val length = strLength(line)
  showInt(length)
  val number = readInt() /* inline */

  showInt(number)
  val decimal = readDouble()
// Fuzz vlozeny komentar
  val decimalText = toStr(decimal)

  print(decimalText)
// Fuzz vlozeny komentar
  print("\n")

// Fuzz vlozeny komentar
def conversions(): Unit = /* inline */
  val fromInt = toStr(42)
  val fromDouble = toStr(3.14159)
  val fromString = toStr("text")
  val fromUnit = toStr(())
  print(fromInt)
  print(" ")
// Fuzz vlozeny komentar

  print(fromDouble)

  print(" ")
  print(fromString)
  print(" ")

  print(fromUnit)
// Fuzz vlozeny komentar
  print("\n")

  val negative = -2.9
  val truncated = toInt(2.9) /* inline */
  val truncatedNegative = toInt(negative)
// Fuzz vlozeny komentar
  showInt(truncated)

  showInt(truncatedNegative)

def stringOperations(): Unit =
  val text = "abcde"
  val index = 1
  val length = strLength(text) /* inline */
  val emptyLength = strLength("")
  showInt(length)
  showInt(emptyLength) /* inline */
  val middle = subStr(text, index, 3)
  val whole = subStr(text, 0, length)
  val nothing_fuzz78 = subStr("xy", 0, 0)
  print(middle)
// Fuzz vlozeny komentar
  print(" ") /* inline */

  print(whole)
  print(" [")

  print(nothing_fuzz78)
// Fuzz vlozeny komentar
  print("]\n")
  val code = ord(text, index)
// Fuzz vlozeny komentar
  val outside = ord(text, length)
  val onEmpty = ord("", 0)
// Fuzz vlozeny komentar
  showInt(code) /* inline */
  showInt(outside)
// Fuzz vlozeny komentar
  showInt(onEmpty)
  val letter = chr(65) /* inline */
// Fuzz vlozeny komentar
  print(letter)
  print("\n")

@main def builtinsExample(): Unit =
  reading()
  readingAtEof()
  conversions()
  stringOperations() /* inline */
