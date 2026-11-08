// Zakladni zadani: vsechny vestavene funkce vcetne chovani pri chybnem vstupu a EOF.
import ifj26.*
// Fuzz vlozeny komentar


def showInt(value: Int): Unit =
  print(value)
// Fuzz vlozeny komentar
  print("\n") /* inline */

def reading(): Unit = /* inline */
  val line = readLine() /* inline */
  print(line)
  val number = readInt()
  showInt(number)
  val decimal = readDouble()
  val decimalText = toStr(decimal)
  print(decimalText)
  print("\n") /* inline */
  val badNumber = readInt()
  showInt(badNumber)
// Fuzz vlozeny komentar

  val badDecimal = readDouble()
// Fuzz vlozeny komentar
  val badDecimalText = toStr(badDecimal)

  print(badDecimalText)

  print("\n")
// Fuzz vlozeny komentar

def readingAtEof(): Unit =
  val line = readLine() /* inline */

  val length = strLength(line)
// Fuzz vlozeny komentar
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
// Fuzz vlozeny komentar
  print("\n") /* inline */
  val negative = -2.9
  val truncated_fuzz83 = toInt(2.9)
  val truncatedNegative = toInt(negative)
  showInt(truncated_fuzz83)
// Fuzz vlozeny komentar
  showInt(truncatedNegative)

// Fuzz vlozeny komentar
def stringOperations(): Unit =
  val text = "abcde"
  val index = 1 /* inline */
  val length = strLength(text)
  val emptyLength = strLength("")
  showInt(length) /* inline */
  showInt(emptyLength)
  val middle = subStr(text, index, 3)
  val whole = subStr(text, 0, length)
  val nothing = subStr("xy", 0, 0)
  print(middle)
  print(" ")
  print(whole)
  print(" [")
  print(nothing)
  print("]\n") /* inline */
  val code = ord(text, index)
  val outside = ord(text, length)
  val onEmpty = ord("", 0) /* inline */
// Fuzz vlozeny komentar
  showInt(code)

  showInt(outside)

  showInt(onEmpty) /* inline */
  val letter = chr(65)
  print(letter)
  print("\n")

@main def builtinsExample(): Unit =
  reading()
  readingAtEof()
  conversions() /* inline */
  stringOperations()
