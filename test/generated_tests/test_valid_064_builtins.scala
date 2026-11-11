// Zakladni zadani: vsechny vestavene funkce vcetne chovani pri chybnem vstupu a EOF.
import ifj26.*

// Fuzz vlozeny komentar
def showInt(value: Int): Unit =
// Fuzz vlozeny komentar

  print(value)

  print("\n")
// Fuzz vlozeny komentar

def reading(): Unit =
  val line = readLine()
  print(line)
  val number = readInt()
  showInt(number)
  val decimal = readDouble()
  val decimalText = toStr(decimal)
  print(decimalText)
// Fuzz vlozeny komentar
  print("\n")
  val badNumber = readInt()
  showInt(badNumber)
// Fuzz vlozeny komentar
  val badDecimal = readDouble()
  val badDecimalText = toStr(badDecimal)
  print(badDecimalText)

  print("\n")

def readingAtEof(): Unit = /* inline */
  val line = readLine() /* inline */
  val length = strLength(line)
  showInt(length)
  val number = readInt() /* inline */
// Fuzz vlozeny komentar
  showInt(number) /* inline */
  val decimal = readDouble()

  val decimalText = toStr(decimal)
  print(decimalText)
  print("\n") /* inline */


def conversions(): Unit =

  val fromInt = toStr(42) /* inline */
// Fuzz vlozeny komentar
  val fromDouble_fuzz30 = toStr(3.14159)

  val fromString = toStr("text")
  val fromUnit = toStr(()) /* inline */
  print(fromInt)

  print(" ")
  print(fromDouble_fuzz30)
  print(" ") /* inline */
  print(fromString)
  print(" ") /* inline */
// Fuzz vlozeny komentar
  print(fromUnit)
// Fuzz vlozeny komentar
  print("\n")
  val negative = -2.9
// Fuzz vlozeny komentar
  val truncated = toInt(2.9) /* inline */
  val truncatedNegative = toInt(negative)
  showInt(truncated)
// Fuzz vlozeny komentar
  showInt(truncatedNegative)

def stringOperations(): Unit = /* inline */
// Fuzz vlozeny komentar
  val text = "abcde" /* inline */
  val index = 1
  val length = strLength(text)
  val emptyLength = strLength("")
  showInt(length)
  showInt(emptyLength) /* inline */
  val middle = subStr(text, index, 3)
  val whole = subStr(text, 0, length)
  val nothing = subStr("xy", 0, 0)
// Fuzz vlozeny komentar
  print(middle)
  print(" ")
  print(whole)
  print(" [")
  print(nothing)
  print("]\n")
  val code = ord(text, index)
  val outside = ord(text, length)
  val onEmpty = ord("", 0)
  showInt(code)
// Fuzz vlozeny komentar
  showInt(outside)
  showInt(onEmpty)
  val letter = chr(65)
  print(letter)
  print("\n")

// Fuzz vlozeny komentar
@main def builtinsExample(): Unit =
  reading()
  readingAtEof()
  conversions()
  stringOperations()
