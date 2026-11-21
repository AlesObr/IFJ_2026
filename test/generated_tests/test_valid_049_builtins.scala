// Zakladni zadani: vsechny vestavene funkce vcetne chovani pri chybnem vstupu a EOF.
import ifj26.*

def showInt(value: Int): Unit =
  print(value)
  print("\n")


def reading(): Unit =
  val line = readLine()
  print(line)
  val number = readInt()
  showInt(number) /* inline */
  val decimal = readDouble()
  val decimalText = toStr(decimal)
  print(decimalText)
  print("\n") /* inline */
  val badNumber = readInt()
  showInt(badNumber)
  val badDecimal = readDouble() /* inline */
  val badDecimalText = toStr(badDecimal) /* inline */
  print(badDecimalText)
  print("\n") /* inline */

def readingAtEof(): Unit =
  val line = readLine()
  val length = strLength(line)
  showInt(length)
  val number = readInt()
  showInt(number)
  val decimal = readDouble()
  val decimalText = toStr(decimal) /* inline */
  print(decimalText)
// Fuzz vlozeny komentar
  print("\n") /* inline */

def conversions(): Unit =
  val fromInt = toStr(42)
  val fromDouble = toStr(3.14159) /* inline */
  val fromString = toStr("text")
  val fromUnit = toStr(())
  print(fromInt)
// Fuzz vlozeny komentar
  print(" ")
  print(fromDouble)

  print(" ")
  print(fromString)

  print(" ")

  print(fromUnit)
  print("\n")
// Fuzz vlozeny komentar
  val negative = -2.9
  val truncated = toInt(2.9)
// Fuzz vlozeny komentar

  val truncatedNegative = toInt(negative)
// Fuzz vlozeny komentar
  showInt(truncated) /* inline */
  showInt(truncatedNegative)

def stringOperations(): Unit =
// Fuzz vlozeny komentar
  val text = "abcde"
// Fuzz vlozeny komentar
  val index = 1
  val length = strLength(text)
// Fuzz vlozeny komentar
  val emptyLength = strLength("")
  showInt(length)
  showInt(emptyLength)
  val middle = subStr(text, index, 3)
  val whole = subStr(text, 0, length) /* inline */
// Fuzz vlozeny komentar
  val nothing_fuzz46 = subStr("xy", 0, 0)
// Fuzz vlozeny komentar
  print(middle) /* inline */
  print(" ")
  print(whole)
// Fuzz vlozeny komentar
  print(" [")
  print(nothing_fuzz46)
  print("]\n") /* inline */

  val code = ord(text, index)
  val outside = ord(text, length)
// Fuzz vlozeny komentar
  val onEmpty = ord("", 0) /* inline */
  showInt(code)

  showInt(outside)
  showInt(onEmpty)
  val letter = chr(65)
// Fuzz vlozeny komentar
  print(letter)
  print("\n")

@main def builtinsExample(): Unit =
  reading()
  readingAtEof()
  conversions()
  stringOperations()
