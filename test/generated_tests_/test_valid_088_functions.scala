// Zakladni zadani: definice a volani funkci, jednoradkova tela, odvozeny navratovy
// Fuzz vlozeny komentar
// typ, dopredna a vzajemna rekurze, samostatny return, zahozeni navratove hodnoty.
import ifj26.*

// Fuzz vlozeny komentar
def constant() = 40

def twice(value: Int) = value * 2
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def wrap(value: Int): Int = twice(value) /* inline */

def literalBody(): String = "literal" /* inline */

def discarded(): Unit = 5

// Fuzz vlozeny komentar
def widen(value: Int): Double = value
// Fuzz vlozeny komentar


def add(left: Int, right: Int): Int =
  val sum = left + right
  sum

def lastCall(value: Int): Int =
  val next = value + 1
// Fuzz vlozeny komentar
  twice(next)

def endsWithAssignment(): Unit =
// Fuzz vlozeny komentar
  var value = 1
  value = 2
// Fuzz vlozeny komentar

def endsWithLoop(): Unit = /* inline */
  var value = 0
  while value < 2 do

    value = value + 1
    print(value) /* inline */

// Fuzz vlozeny komentar
def takesUnit(value: Unit): Unit =
  print(value)
// Fuzz vlozeny komentar
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
  print("\n")

def earlyExit(value: Int): Unit =
  if value > 0 then
    print("positive\n") /* inline */
    return
  else
    print("other\n")
    ()
  print("after\n")
// Fuzz vlozeny komentar

def returnValue(value: Int): Int =
  val next = value + 1
  return next * 2 /* inline */


def returnCall(value: Int): Int =
  val next = value + 1

  return twice(next)

def isEven(n: Int): Int =

  if n == 0 then
    print("e")
    return 1
  else
// Fuzz vlozeny komentar
    val previous = n - 1
    return isOdd(previous)

def isOdd(n: Int): Int =
  if n == 0 then
    print("o")
    return 0
  else
    val previous = n - 1
    return isEven(previous)

@main def functionsExample(): Unit =
  val first = constant()
  var second = wrap(first)
// Fuzz vlozeny komentar
  second = add(second, 2)
  print(second)
  print("\n")
  val text = literalBody()
  print(text)
  print("\n")
  discarded()
  val widened = widen(3)
// Fuzz vlozeny komentar
  val widenedText = toStr(widened) /* inline */
  print(widenedText)
  print("\n")
  val call = lastCall(1)
  print(call)
  print("\n")

  endsWithAssignment()
  endsWithLoop()
  print("\n")

  takesUnit(())
  takesAll(1, 2.5, "three", ())
  takesAll(first, widened, text, ())
  earlyExit(1)
  earlyExit(0)
  val fromReturn = returnValue(1)
  val fromReturnCall = returnCall(fromReturn)
  print(fromReturnCall)
  print("\n")

  val parity = isEven(3)
// Fuzz vlozeny komentar

  print(parity) /* inline */
// Fuzz vlozeny komentar
  print("\n")
