// Zakladni zadani: vsechny vestavene funkce vcetne chovani pri chybnem vstupu a EOF.
import ifj26.* /* inline */

// Fuzz vlozeny komentar
def showInt(value: Int): Unit =
  print(value) /* inline */
  print("\n")

def reading(): Unit = /* inline */
  val line = readLine()
  print(line)

  val number = readInt() /* inline */
  showInt(number)

  val decimal = readDouble()
  val decimalText = toStr(decimal)
  print(decimalText)
  print("\n")
  val badNumber = readInt()
  showInt(badNumber)
  val badDecimal = readDouble()
  val badDecimalText = toStr(badDecimal)
  print(badDecimalText)
// Fuzz vlozeny komentar
  print("\n")

// Fuzz vlozeny komentar
def readingAtEof(): Unit = /* inline */
  val line = readLine()

  val length = strLength(line)
  showInt(length)

  val number = readInt()
  showInt(number)
  val decimal = readDouble()
  val decimalText = toStr(decimal)
  print(decimalText)

  print("\n")

// Fuzz vlozeny komentar
def conversions(): Unit =
  val fromInt = toStr(42)
  val fromDouble = toStr(3.14159)
  val fromString = toStr("text")
  val fromUnit = toStr(())
// Fuzz vlozeny komentar
  print(fromInt)
  print(" ")
// Fuzz vlozeny komentar
  print(fromDouble)
  print(" ")
// Fuzz vlozeny komentar
  print(fromString)
  print(" ")
  print(fromUnit)
  print("\n")
  val negative = -2.9
// Fuzz vlozeny komentar
  val truncated = toInt(2.9)
// Fuzz vlozeny komentar
  val truncatedNegative = toInt(negative)
  showInt(truncated) /* inline */
  showInt(truncatedNegative)

def stringOperations(): Unit =
  val text = "abcde"
  val index = 1
  val length = strLength(text)
  val emptyLength = strLength("")
  showInt(length)
  showInt(emptyLength)
// Fuzz vlozeny komentar
  val middle = subStr(text, index, 3) /* inline */
  val whole = subStr(text, 0, length)
  val nothing = subStr("xy", 0, 0)
  print(middle)
  print(" ")
  print(whole)
  print(" [") /* inline */
  print(nothing)
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

@main def builtinsExample(): Unit =
// Fuzz vlozeny komentar
  reading()
  readingAtEof() /* inline */
  conversions() /* inline */
// Fuzz vlozeny komentar
  stringOperations()
