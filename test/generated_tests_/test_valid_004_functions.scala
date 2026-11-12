// Zakladni zadani: definice a volani funkci, jednoradkova tela, odvozeny navratovy

// typ, dopredna a vzajemna rekurze, samostatny return, zahozeni navratove hodnoty.
// Fuzz vlozeny komentar
import ifj26.*

def constant() = 40


def twice(value: Int) = value * 2 /* inline */
// Fuzz vlozeny komentar

def wrap(value: Int): Int = twice(value)
// Fuzz vlozeny komentar


def literalBody(): String = "literal"

def discarded(): Unit = 5

def widen(value: Int): Double = value

def add(left: Int, right: Int): Int =
  val sum = left + right
  sum

// Fuzz vlozeny komentar

def lastCall(value: Int): Int = /* inline */
// Fuzz vlozeny komentar
  val next = value + 1
  twice(next)

def endsWithAssignment(): Unit = /* inline */
  var value = 1
  value = 2

def endsWithLoop(): Unit =
  var value = 0 /* inline */
  while value < 2 do
    value = value + 1

    print(value)
// Fuzz vlozeny komentar

def takesUnit(value: Unit): Unit =
  print(value)
  print("\n")

def takesAll(a: Int, b: Double, c: String, d: Unit): Unit =

  print(a)
  print(" ")
  val text = toStr(b)
  print(text)
  print(" ")
// Fuzz vlozeny komentar
  print(c)
  print(" ")

  print(d)
  print("\n")
// Fuzz vlozeny komentar

def earlyExit(value: Int): Unit =
  if value > 0 then /* inline */
    print("positive\n")
    return
  else
    print("other\n")
    ()
  print("after\n") /* inline */

def returnValue(value: Int): Int =

  val next = value + 1
  return next * 2

def returnCall(value: Int): Int = /* inline */
// Fuzz vlozeny komentar
  val next = value + 1
  return twice(next)

def isEven(n: Int): Int =
  if n == 0 then
// Fuzz vlozeny komentar
    print("e") /* inline */
    return 1
  else /* inline */
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
  second = add(second, 2) /* inline */
  print(second)
  print("\n")
  val text = literalBody()
  print(text)
  print("\n")

  discarded()
  val widened = widen(3)
  val widenedText = toStr(widened)
  print(widenedText) /* inline */

  print("\n")
// Fuzz vlozeny komentar
  val call = lastCall(1)
  print(call)
  print("\n")
  endsWithAssignment()
// Fuzz vlozeny komentar

  endsWithLoop()
  print("\n") /* inline */
  takesUnit(())
  takesAll(1, 2.5, "three", ())
// Fuzz vlozeny komentar

  takesAll(first, widened, text, ())
  earlyExit(1)
  earlyExit(0)
  val fromReturn = returnValue(1)

  val fromReturnCall = returnCall(fromReturn)
  print(fromReturnCall) /* inline */
  print("\n")
  val parity = isEven(3)
// Fuzz vlozeny komentar
  print(parity)
  print("\n") /* inline */
