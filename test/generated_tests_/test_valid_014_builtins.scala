// Zakladni zadani: vsechny vestavene funkce vcetne chovani pri chybnem vstupu a EOF.
// Fuzz vlozeny komentar
import ifj26.*

def showInt(value: Int): Unit =
  print(value)
  print("\n") /* inline */

def reading(): Unit =
  val line = readLine() /* inline */
  print(line)
  val number = readInt()
  showInt(number)
  val decimal = readDouble()
  val decimalText = toStr(decimal)
// Fuzz vlozeny komentar
  print(decimalText)
  print("\n")
  val badNumber = readInt()
  showInt(badNumber)
  val badDecimal = readDouble()
  val badDecimalText = toStr(badDecimal)
  print(badDecimalText)
  print("\n")

// Fuzz vlozeny komentar
def readingAtEof(): Unit =
  val line = readLine()
// Fuzz vlozeny komentar
  val length = strLength(line)
  showInt(length)
  val number = readInt()
  showInt(number)
// Fuzz vlozeny komentar
  val decimal = readDouble()
  val decimalText = toStr(decimal)
  print(decimalText)
  print("\n")
// Fuzz vlozeny komentar

def conversions(): Unit =
  val fromInt = toStr(42)
// Fuzz vlozeny komentar
  val fromDouble = toStr(3.14159)
// Fuzz vlozeny komentar
  val fromString = toStr("text")
  val fromUnit = toStr(())
  print(fromInt)
  print(" ")
  print(fromDouble)
  print(" ")
  print(fromString) /* inline */

  print(" ")
  print(fromUnit)
// Fuzz vlozeny komentar
  print("\n")

  val negative = -2.9 /* inline */
// Fuzz vlozeny komentar
  val truncated = toInt(2.9)
  val truncatedNegative = toInt(negative) /* inline */

  showInt(truncated)
  showInt(truncatedNegative)
// Fuzz vlozeny komentar

def stringOperations(): Unit =
  val text = "abcde"

  val index = 1
  val length = strLength(text) /* inline */
  val emptyLength = strLength("")
  showInt(length)
  showInt(emptyLength)
// Fuzz vlozeny komentar
  val middle = subStr(text, index, 3)
  val whole = subStr(text, 0, length)
  val nothing = subStr("xy", 0, 0) /* inline */
  print(middle)

  print(" ")
  print(whole)
  print(" [") /* inline */
  print(nothing)
// Fuzz vlozeny komentar
  print("]\n")
  val code_fuzz49 = ord(text, index) /* inline */
  val outside = ord(text, length)
  val onEmpty = ord("", 0)
  showInt(code_fuzz49)
// Fuzz vlozeny komentar
  showInt(outside)
  showInt(onEmpty) /* inline */

  val letter = chr(65)
  print(letter) /* inline */
  print("\n")
// Fuzz vlozeny komentar

@main def builtinsExample(): Unit =
  reading()
// Fuzz vlozeny komentar
  readingAtEof()
  conversions()

  stringOperations()
