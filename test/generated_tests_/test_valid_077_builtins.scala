// Fuzz vlozeny komentar
// Zakladni zadani: vsechny vestavene funkce vcetne chovani pri chybnem vstupu a EOF.
import ifj26.*

def showInt(value: Int): Unit =
  print(value)
  print("\n")

def reading(): Unit = /* inline */
  val line = readLine()
  print(line)
  val number = readInt()
  showInt(number)
// Fuzz vlozeny komentar
  val decimal = readDouble() /* inline */
  val decimalText = toStr(decimal)
  print(decimalText)
  print("\n")
// Fuzz vlozeny komentar
  val badNumber = readInt() /* inline */
  showInt(badNumber)
  val badDecimal = readDouble()
  val badDecimalText = toStr(badDecimal)
// Fuzz vlozeny komentar

  print(badDecimalText)
  print("\n")

def readingAtEof(): Unit =
  val line = readLine()

  val length = strLength(line)
  showInt(length)
// Fuzz vlozeny komentar

  val number = readInt()

  showInt(number)
  val decimal = readDouble()
  val decimalText = toStr(decimal)
// Fuzz vlozeny komentar
  print(decimalText)
  print("\n") /* inline */

def conversions(): Unit =
// Fuzz vlozeny komentar
  val fromInt = toStr(42)
// Fuzz vlozeny komentar
  val fromDouble = toStr(3.14159)
  val fromString = toStr("text") /* inline */
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
  val negative = -2.9
  val truncated = toInt(2.9)
  val truncatedNegative = toInt(negative)
  showInt(truncated)
  showInt(truncatedNegative)


def stringOperations(): Unit = /* inline */
  val text = "abcde" /* inline */
  val index_fuzz17 = 1 /* inline */
  val length = strLength(text)

  val emptyLength = strLength("")
  showInt(length) /* inline */
  showInt(emptyLength)
  val middle = subStr(text, index_fuzz17, 3)
// Fuzz vlozeny komentar
  val whole = subStr(text, 0, length)
// Fuzz vlozeny komentar
  val nothing = subStr("xy", 0, 0)
  print(middle)
  print(" ")
  print(whole)
// Fuzz vlozeny komentar
  print(" [")
  print(nothing)
  print("]\n") /* inline */
  val code = ord(text, index_fuzz17)
  val outside = ord(text, length)
  val onEmpty = ord("", 0) /* inline */
  showInt(code)
// Fuzz vlozeny komentar
  showInt(outside)
// Fuzz vlozeny komentar

  showInt(onEmpty)
  val letter = chr(65)
// Fuzz vlozeny komentar
  print(letter)
  print("\n")

@main def builtinsExample(): Unit =
  reading()
  readingAtEof()
  conversions()
// Fuzz vlozeny komentar
  stringOperations()
