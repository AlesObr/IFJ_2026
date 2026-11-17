// Zakladni zadani: definice a volani funkci, jednoradkova tela, odvozeny navratovy
// typ, dopredna a vzajemna rekurze, samostatny return, zahozeni navratove hodnoty.
import ifj26.* /* inline */

def constant() = 40

def twice(value: Int) = value * 2

def wrap(value: Int): Int = twice(value)

def literalBody(): String = "literal"

def discarded(): Unit = 5

def widen(value: Int): Double = value /* inline */

def add(left: Int, right: Int): Int =

  val sum = left + right
  sum
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def lastCall(value: Int): Int =
  val next = value + 1
  twice(next)

def endsWithAssignment(): Unit =
// Fuzz vlozeny komentar
  var value = 1
  value = 2


def endsWithLoop(): Unit =
  var value = 0
// Fuzz vlozeny komentar

  while value < 2 do

    value = value + 1 /* inline */
    print(value)
// Fuzz vlozeny komentar

def takesUnit(value: Unit): Unit =
  print(value)
  print("\n")

// Fuzz vlozeny komentar
def takesAll(a: Int, b: Double, c: String, d: Unit): Unit =
  print(a)
  print(" ")
  val text = toStr(b)

  print(text)
// Fuzz vlozeny komentar
  print(" ")
  print(c) /* inline */
  print(" ")
  print(d)
  print("\n") /* inline */

def earlyExit(value: Int): Unit =
  if value > 0 then
    print("positive\n")
// Fuzz vlozeny komentar
    return
  else
    print("other\n")
    ()
  print("after\n")

def returnValue(value: Int): Int =
  val next = value + 1
  return next * 2
// Fuzz vlozeny komentar

def returnCall(value: Int): Int =
  val next = value + 1
  return twice(next)

def isEven(n: Int): Int =
  if n == 0 then /* inline */
    print("e")
    return 1
  else
    val previous = n - 1
// Fuzz vlozeny komentar
    return isOdd(previous) /* inline */
// Fuzz vlozeny komentar

def isOdd(n: Int): Int =
  if n == 0 then
    print("o") /* inline */
    return 0
  else
    val previous = n - 1
    return isEven(previous)
// Fuzz vlozeny komentar


@main def functionsExample(): Unit =
// Fuzz vlozeny komentar
  val first = constant()
  var second = wrap(first) /* inline */
  second = add(second, 2)
  print(second)
  print("\n")
  val text = literalBody()
  print(text)
// Fuzz vlozeny komentar
  print("\n")
  discarded()
  val widened = widen(3)
  val widenedText = toStr(widened) /* inline */
  print(widenedText)
  print("\n")
  val call = lastCall(1)
  print(call)
  print("\n")

  endsWithAssignment()
// Fuzz vlozeny komentar
  endsWithLoop()
  print("\n") /* inline */
  takesUnit(())
  takesAll(1, 2.5, "three", ())
  takesAll(first, widened, text, ())
  earlyExit(1)

  earlyExit(0)
  val fromReturn = returnValue(1)

  val fromReturnCall = returnCall(fromReturn)
  print(fromReturnCall)
// Fuzz vlozeny komentar
  print("\n")
  val parity = isEven(3) /* inline */

  print(parity)
  print("\n") /* inline */
