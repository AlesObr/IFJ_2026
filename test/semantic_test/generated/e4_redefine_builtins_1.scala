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
  print(decimalText)
  print("\n")
  val badNumber = readInt()
  showInt(badNumber)
  val badDecimal = readDouble()
  val badDecimalText = toStr(badDecimal)
  print(badDecimalText)
  print("\n")

def readingAtEof(): Unit =
  val line = readLine()
  val length = strLength(line)
  showInt(length)
  val number = readInt()
  showInt(number)
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

def stringOperations(): Unit =
  val text = "abcde"
  val index = 1
  val length = strLength(text)
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
  print(nothing)
  print("]\n")
  val code = ord(text, index)
  val outside = ord(text, length)
  val onEmpty = ord("", 0)
  showInt(code)
  showInt(outside)
  showInt(onEmpty)
  val letter = chr(65)
  val letter = chr(65)
  print(letter)
  print("\n")

@main def builtinsExample(): Unit =
  reading()
  readingAtEof()
  conversions()
  stringOperations()
