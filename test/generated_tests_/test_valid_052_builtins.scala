// Zakladni zadani: vsechny vestavene funkce vcetne chovani pri chybnem vstupu a EOF.
import ifj26.*

def showInt(value: Int): Unit =
  print(value)
  print("\n")

def reading(): Unit =
  val line = readLine()
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


def readingAtEof(): Unit = /* inline */
  val line = readLine()
  val length = strLength(line)
// Fuzz vlozeny komentar
  showInt(length)
  val number = readInt()
  showInt(number) /* inline */
  val decimal = readDouble()
  val decimalText = toStr(decimal)

  print(decimalText)
  print("\n")

def conversions(): Unit =
  val fromInt = toStr(42)

  val fromDouble = toStr(3.14159)
  val fromString = toStr("text")
  val fromUnit = toStr(())
  print(fromInt)
  print(" ") /* inline */
// Fuzz vlozeny komentar
  print(fromDouble)
  print(" ")
// Fuzz vlozeny komentar

  print(fromString)
  print(" ")
  print(fromUnit)
  print("\n")
// Fuzz vlozeny komentar
  val negative = -2.9 /* inline */
  val truncated = toInt(2.9) /* inline */
  val truncatedNegative = toInt(negative) /* inline */
// Fuzz vlozeny komentar
  showInt(truncated)
  showInt(truncatedNegative)


def stringOperations(): Unit =
  val text = "abcde"
  val index = 1

  val length = strLength(text)
  val emptyLength = strLength("")
  showInt(length)
  showInt(emptyLength)

  val middle = subStr(text, index, 3)
  val whole = subStr(text, 0, length) /* inline */
  val nothing = subStr("xy", 0, 0)
// Fuzz vlozeny komentar
  print(middle)
  print(" ") /* inline */
  print(whole) /* inline */
  print(" [")
  print(nothing)
  print("]\n")
// Fuzz vlozeny komentar
  val code = ord(text, index)
  val outside = ord(text, length) /* inline */
// Fuzz vlozeny komentar
  val onEmpty = ord("", 0)
  showInt(code)

  showInt(outside) /* inline */

  showInt(onEmpty)
  val letter = chr(65)
  print(letter)
  print("\n")
// Fuzz vlozeny komentar

@main def builtinsExample_fuzz92(): Unit =
  reading()
  readingAtEof()
  conversions()
  stringOperations()
