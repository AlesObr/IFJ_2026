
// Zakladni zadani: vsechny vestavene funkce vcetne chovani pri chybnem vstupu a EOF.
import ifj26.*

def showInt(value: Int): Unit =
  
  print(value) ;  print("\n")

def reading(): Unit =

  val line = readLine()
  
  print(line)
// Fuzz vlozeny komentar
  val number = readInt()
  showInt(number)
  val decimal = readDouble()
  val decimalText = toStr(decimal) ;  print(decimalText)  
  print("\n")
  
  val badNumber = readInt() /* inline */
  showInt(badNumber)
  val badDecimal = readDouble()
            
  val badDecimalText = toStr(badDecimal) /* inline */
          
  print(badDecimalText) ;  print("\n")
 
def readingAtEof(): Unit =
// Fuzz vlozeny komentar
  val line = readLine() ;  val length = strLength(line)
  showInt(length)
// Fuzz vlozeny komentar  
  val number = readInt()
  showInt(number) /* inline */
    
  val decimal = readDouble()
          
  val decimalText = toStr(decimal)
  print(decimalText)
// Fuzz vlozeny komentar
  print("\n")
    
def conversions(): Unit =
// Fuzz vlozeny komentar
  val fromInt = toStr(42)    
// Fuzz vlozeny komentar

  val fromDouble = toStr(3.14159)
    
  val fromString = toStr("text")
  val fromUnit = toStr(()) /* inline */
  print(fromInt) /* inline */
// Fuzz vlozeny komentar

       
  print(" ");print(fromDouble)

  print(" ")
// Fuzz vlozeny komentar
  print(fromString)
// Fuzz vlozeny komentar
  print(" ") /* inline */
  print(fromUnit)
  print("\n")
  val negative = -2.9 /* inline */
  val truncated = toInt(2.9)
  val truncatedNegative = toInt(negative)
  showInt(truncated) /* inline */    
  showInt(truncatedNegative)

def stringOperations(): Unit =
// Fuzz vlozeny komentar
  val text = "abcde" /* inline */
  val index = 1 /* inline */
  val length = strLength(text) ;  val emptyLength = strLength("")
  showInt(length) ;  showInt(emptyLength)
  val middle = subStr(text, index, 3)
// Fuzz vlozeny komentar  
  val whole = subStr(text, 0, length)
  val nothing = subStr("xy", 0, 0)
  print(middle) /* inline */
   
  print(" ")
  print(whole); print(" [")
  print(nothing)
  print("]\n")   

  val code = ord(text, index)
  val outside = ord(text, length)
// Fuzz vlozeny komentar
    

  val onEmpty = ord("", 0) /* inline */
  showInt(code)

  showInt(outside); showInt(onEmpty)  
  val letter = chr(65) /* inline */
  print(letter) /* inline */
  print("\n")

@main def builtinsExample(): Unit =

  reading()

  readingAtEof()
// Fuzz vlozeny komentar
  conversions()  
  stringOperations()
