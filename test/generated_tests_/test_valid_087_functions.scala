// Zakladni zadani: definice a volani funkci, jednoradkova tela, odvozeny navratovy

// typ, dopredna a vzajemna rekurze, samostatny return, zahozeni navratove hodnoty.
import ifj26.*

def constant() = 40
// Fuzz vlozeny komentar

def twice(value: Int) = value * 2

def wrap(value: Int): Int = twice(value)

def literalBody(): String = "literal"


def discarded(): Unit = 5

def widen(value: Int): Double = value

def add(left: Int, right: Int): Int = /* inline */

  val sum_fuzz94 = left + right
  sum_fuzz94

def lastCall(value: Int): Int =
  val next = value + 1

  twice(next)

def endsWithAssignment(): Unit =
// Fuzz vlozeny komentar
  var value = 1
  value = 2

def endsWithLoop(): Unit =
  var value = 0
  while value < 2 do
    value = value + 1

    print(value)

def takesUnit(value: Unit): Unit =
// Fuzz vlozeny komentar
  print(value)
// Fuzz vlozeny komentar
  print("\n")

def takesAll(a: Int, b: Double, c: String, d: Unit): Unit =
  print(a)

  print(" ")
  val text = toStr(b)
// Fuzz vlozeny komentar
  print(text)
  print(" ")
  print(c)
  print(" ") /* inline */
  print(d)
  print("\n")

def earlyExit(value: Int): Unit =
  if value > 0 then
// Fuzz vlozeny komentar
    print("positive\n")
    return /* inline */
// Fuzz vlozeny komentar

  else
    print("other\n")
    () /* inline */
  print("after\n")

def returnValue(value: Int): Int =
  val next = value + 1
  return next * 2

def returnCall(value: Int): Int =
  val next = value + 1
  return twice(next)

def isEven(n: Int): Int =

  if n == 0 then
    print("e")
    return 1 /* inline */
  else
    val previous = n - 1
    return isOdd(previous)

def isOdd(n: Int): Int =
  if n == 0 then
    print("o")
    return 0
  else /* inline */
    val previous = n - 1
    return isEven(previous) /* inline */

@main def functionsExample(): Unit =
  val first = constant()
  var second = wrap(first)
  second = add(second, 2)
  print(second) /* inline */
  print("\n")
  val text = literalBody()
  print(text) /* inline */
  print("\n") /* inline */
// Fuzz vlozeny komentar
  discarded()
  val widened = widen(3)
  val widenedText = toStr(widened)
  print(widenedText)
  print("\n")
  val call = lastCall(1)
  print(call)
  print("\n")
  endsWithAssignment()
  endsWithLoop()
  print("\n")
  takesUnit(())
// Fuzz vlozeny komentar
  takesAll(1, 2.5, "three", ())
  takesAll(first, widened, text, ())
  earlyExit(1)
  earlyExit(0) /* inline */

  val fromReturn = returnValue(1)
// Fuzz vlozeny komentar
  val fromReturnCall = returnCall(fromReturn) /* inline */
  print(fromReturnCall)
  print("\n") /* inline */
  val parity = isEven(3) /* inline */
  print(parity)
  print("\n")
