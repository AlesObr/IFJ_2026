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

def conversions(
