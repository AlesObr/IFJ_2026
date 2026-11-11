// Fuzz vlozeny komentar

// Zakladni zadani: vsechny vestavene funkce vcetne chovani pri chybnem vstupu a EOF.
import ifj26.*

def showInt(value: Int): Unit =
// Fuzz vlozeny komentar
  print(value)
// Fuzz vlozeny komentar
  print("\n")

def reading(): Unit =
  val line = readLine() /* inline */
// Fuzz vlozeny komentar
  print(line)
  val number = readInt()
  showInt(number) /* inline */
  val decimal = readDouble() /* inline */
  val decimalText = toStr(decimal)
  print(decimalText)
  print("\n")
  val badNumber = readInt()
  showInt(badNumber)
  val badDecimal = readDouble()
  val badDecimalText = toStr(badDecimal)
  print(badDecimalText)

  print("\n")

def readingAtEof(): Unit =
  val line = readLine() /* inline */
// Fuzz vlozeny komentar
  val length = strLength(line) /* inline */
  showInt(length)
  val number = readInt()
  showInt(number)
  val decimal = readDouble()

  val decimalText = toStr(decimal)
  print(decimalText)
// Fuzz vlozeny komentar
  print("\n")
// Fuzz vlozeny komentar

def conversions(): Unit =
  val fromInt = toStr(42)
// Fuzz vlozeny komentar
  val fromDouble = toStr(3.14159)
  val fromString = toStr("text")
  val fromUnit = toStr(())
  print(fromInt)
  print(" ")
  print(fromDouble)
  print(" ") /* inline */
  print(fromString)
  print(" ")

  print(fromUnit)
  print("\n") /* inline */
  val negative = -2.9
  val truncated = toInt(2.9)
  val truncatedNegative = toInt(negative)
  showInt(truncated)
  showInt(truncatedNegative)

def stringOperations(): Unit =
  val text = "abcde"

  val index = 1
  val length = strLength(text)
  val emptyLength = strLength("") /* inline */
  showInt(length)
  showInt(emptyLength)
  val middle = subStr(text, index, 3)
// Fuzz vlozeny komentar
  val whole = subStr(text, 0, length)
// Fuzz vlozeny komentar
  val nothing = subStr("xy", 0, 0)
  print(middle) /* inline */
  print(" ")
  print(whole)
  print(" [")
  print(nothing)
  print("]\n")
  val code_fuzz98 = ord(text, index)
  val outside = ord(text, length)
  val onEmpty = ord("", 0)
  showInt(code_fuzz98)
  showInt(outside)

  showInt(onEmpty)
  val letter = chr(65)
  print(letter)
  print("\n")

@main def builtinsExample(): Unit =
  reading()
  readingAtEof()
  conversions()
// Fuzz vlozeny komentar
  stringOperations()
