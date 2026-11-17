// Fuzz vlozeny komentar
// Zakladni zadani: definice a volani funkci, jednoradkova tela, odvozeny navratovy
// Fuzz vlozeny komentar
// typ, dopredna a vzajemna rekurze, samostatny return, zahozeni navratove hodnoty.
import ifj26.*

def constant() = 40

def twice(value: Int) = value * 2

def wrap(value: Int): Int = twice(value)

def literalBody(): String = "literal"

def discarded(): Unit = 5

def widen(value: Int): Double = value

// Fuzz vlozeny komentar
def add(left: Int, right: Int): Int =
  val sum = left + right
  sum

def lastCall(value: Int): Int =
  val next = value + 1
  twice(next)
// Fuzz vlozeny komentar


def endsWithAssignment(): Unit =
  var value = 1
  value = 2

def endsWithLoop(): Unit =
// Fuzz vlozeny komentar
  var value = 0
  while value < 2 do
    value = value + 1
    print(value)

def takesUnit(value: Unit): Unit =

  print(value)
  print("\n")


def takesAll(a: Int, b: Double, c: String, d: Unit): Unit =
  print(a)
  print(" ")
  val text = toStr(b)

  print(text)
  print(" ")
  print(c)
  print(" ")

  print(d)
// Fuzz vlozeny komentar
  print("\n")

def earlyExit(value: Int): Unit =
  if value > 0 then
    print("positive\n")
    return
// Fuzz vlozeny komentar
  else
    print("other\n")
    () /* inline */
  print("after\n")

// Fuzz vlozeny komentar
def returnValue(value: Int): Int =
  val next = value + 1
  return next * 2

def returnCall(value: Int): Int =
  val next = value + 1
  return twice(next)

def isEven(n: Int): Int =

  if n == 0 then
    print("e")
// Fuzz vlozeny komentar
    return 1
  else
    val previous = n - 1
// Fuzz vlozeny komentar
    return isOdd(previous) /* inline */


def isOdd(n: Int): Int =
  if n == 0 then
    print("o")
    return 0
// Fuzz vlozeny komentar
  else
    val previous = n - 1
    return isEven(previous)

// Fuzz vlozeny komentar
@main def functionsExample(): Unit =
  val first = constant()
  var second = wrap(first) /* inline */
  second = add(second, 2)
  print(second) /* inline */
  print("\n")
  val text = literalBody()
  print(text)
  print("\n")
  discarded()
  val widened = widen(3) /* inline */
  val widenedText = toStr(widened)
  print(widenedText)
  print("\n") /* inline */
  val call = lastCall(1)
  print(call)
// Fuzz vlozeny komentar
  print("\n")
  endsWithAssignment()

  endsWithLoop()
  print("\n")
// Fuzz vlozeny komentar
  takesUnit(())
  takesAll(1, 2.5, "three", ()) /* inline */
  takesAll(first, widened, text, ())
// Fuzz vlozeny komentar
  earlyExit(1)

  earlyExit(0)
// Fuzz vlozeny komentar
  val fromReturn_fuzz95 = returnValue(1)
// Fuzz vlozeny komentar
  val fromReturnCall = returnCall(fromReturn_fuzz95)
  print(fromReturnCall)
  print("\n")
  val parity = isEven(3)
  print(parity)
// Fuzz vlozeny komentar
  print("\n")
