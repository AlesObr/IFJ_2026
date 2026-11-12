// Zakladni zadani: vsechny vestavene funkce vcetne chovani pri chybnem vstupu a EOF. /* inline */
import ifj26.*

def showInt(value: Int): Unit =
  print(value)

  print(("\n"))

def reading(): Unit =
// Fuzz vlozeny komentar
  val line = readLine() /* inline */
  print(line)
// Fuzz vlozeny komentar
  val number = readInt(); showInt(number)
  val decimal = readDouble()

  val decimalText = toStr(decimal);print(decimalText)

  print("\n")
  val badNumber = readInt()
  showInt(badNumber) /* inline */
  val badDecimal = readDouble()
  val badDecimalText = toStr(badDecimal)
  print(badDecimalText)
  print("\n") /* inline */


def readingAtEof(): Unit =
  val line = readLine();val length = strLength(line)
  showInt(length)
  val number = readInt()
// Fuzz vlozeny komentar
  showInt(number) /* inline */
  val decimal = readDouble()
  val decimalText = toStr(decimal); print(decimalText)
  print("\n")
// Fuzz vlozeny komentar

def conversions(): Unit =
  val fromInt = toStr(42)
  val fromDouble = toStr(3.14159)
  val fromString = toStr("text")
  val fromUnit = toStr(())
  print(fromInt) ;  print(" ")
  print(fromDouble) /* inline */
  print(" ")

  print(fromString) /* inline */
  print(" ")
// Fuzz vlozeny komentar
  print(fromUnit)
  print("\n") ;  val negative = -2.9
  val truncated = toInt((2.9))
// Fuzz vlozeny komentar
  val truncatedNegative = toInt(negative);showInt(truncated)
  showInt(truncatedNegative)

def stringOperations(): Unit = /* inline */
  val text = "abcde"
  val index = 1;val length = strLength(text)
  val emptyLength = strLength("") /* inline */
  showInt(length); showInt(emptyLength)
  val middle = subStr(text, index, 3)
  val whole = subStr(text, 0, length) /* inline */
  val nothing = subStr(("xy"), 0, 0)
  print(middle);print(" ")
// Fuzz vlozeny komentar

  print(whole)
  print(" [")
// Fuzz vlozeny komentar
  print(nothing)
  print("]\n") /* inline */
  val code = ord(text, index) ;  val outside = ord(text, length)

  val onEmpty = ord("", (0))
// Fuzz vlozeny komentar
  showInt(code)
  showInt(outside)
  showInt(onEmpty)
  val letter = chr(65)
  print(letter);print(("\n"))


// Fuzz vlozeny komentar
@main def builtinsExample(): Unit =
  reading()
  readingAtEof() /* inline */
  conversions()
  stringOperations()
