// Fuzz vlozeny komentar
// Zakladni zadani: vsechny vestavene funkce vcetne chovani pri chybnem vstupu a EOF.
// Fuzz vlozeny komentar
import ifj26.*

// Fuzz vlozeny komentar
def showInt(value: Int): Unit =
  print(value)
  print("\n")

def reading(): Unit = /* inline */
  val line = readLine()
  print(line)
  val number = readInt()
// Fuzz vlozeny komentar
  showInt(number)
  val decimal = readDouble()
  val decimalText = toStr(decimal)
  print(decimalText)
  print("\n")
  val badNumber = readInt() /* inline */

  showInt(badNumber)
  val badDecimal_fuzz87 = readDouble()
  val badDecimalText = toStr(badDecimal_fuzz87)
  print(badDecimalText)

  print("\n")
// Fuzz vlozeny komentar

def readingAtEof(): Unit = /* inline */
  val line = readLine()
// Fuzz vlozeny komentar
  val length = strLength(line)
  showInt(length) /* inline */
  val number = readInt()
  showInt(number)
  val decimal = readDouble()
  val decimalText = toStr(decimal)
  print(decimalText)
// Fuzz vlozeny komentar
  print("\n")

def conversions(): Unit =
  val fromInt = toStr(42)
  val fromDouble = toStr(3.14159)
  val fromString = toStr("text")
  val fromUnit = toStr(())
  print(fromInt)
  print(" ")
// Fuzz vlozeny komentar
  print(fromDouble)
  print(" ")
  print(fromString) /* inline */
  print(" ")
  print(fromUnit)
  print("\n")
// Fuzz vlozeny komentar
  val negative = -2.9
  val truncated = toInt(2.9) /* inline */
  val truncatedNegative = toInt(negative)
  showInt(truncated)
  showInt(truncatedNegative)


def stringOperations(): Unit =
// Fuzz vlozeny komentar
  val text = "abcde"
  val index = 1 /* inline */
  val length = strLength(text)
// Fuzz vlozeny komentar

  val emptyLength = strLength("")
// Fuzz vlozeny komentar
  showInt(length)
  showInt(emptyLength)
  val middle = subStr(text, index, 3)
  val whole = subStr(text, 0, length) /* inline */
  val nothing = subStr("xy", 0, 0)

  print(middle)
  print(" ")

  print(whole) /* inline */
// Fuzz vlozeny komentar
  print(" [") /* inline */
// Fuzz vlozeny komentar

  print(nothing)
  print("]\n")
// Fuzz vlozeny komentar
  val code = ord(text, index) /* inline */
  val outside = ord(text, length)
// Fuzz vlozeny komentar
  val onEmpty = ord("", 0)
  showInt(code)
  showInt(outside)
// Fuzz vlozeny komentar
  showInt(onEmpty)
  val letter = chr(65)

  print(letter)
  print("\n")

@main def builtinsExample(): Unit =
// Fuzz vlozeny komentar
  reading()
// Fuzz vlozeny komentar
  readingAtEof()
  conversions()
  stringOperations()
