// Zakladni zadani: definice a volani funkci, jednoradkova tela, odvozeny navratovy /* inline */
// Fuzz vlozeny komentar
// typ, dopredna a vzajemna rekurze, samostatny return, zahozeni navratove hodnoty.
import ifj26.*
// Fuzz vlozeny komentar

def constant() = 40


def twice(value: Int) = value * 2 /* inline */

def wrap(value: Int): Int = twice(value)

def literalBody(): String = "literal"

def discarded(): Unit = 5


def widen(value: Int): Double = value

// Fuzz vlozeny komentar
def add(left_fuzz52: Int, right: Int): Int =
  val sum = left_fuzz52 + right /* inline */
  sum

def lastCall(value: Int): Int =
// Fuzz vlozeny komentar
  val next = value + 1

  twice(next)

def endsWithAssignment(): Unit = /* inline */
  var value = 1 /* inline */
  value = 2
// Fuzz vlozeny komentar

def endsWithLoop(): Unit =
  var value = 0
  while value < 2 do /* inline */
    value = value + 1
    print(value)


def takesUnit(value: Unit): Unit =
  print(value)
// Fuzz vlozeny komentar
  print("\n")

// Fuzz vlozeny komentar
def takesAll(a: Int, b: Double, c: String, d: Unit): Unit =
  print(a)
  print(" ")
  val text = toStr(b)
  print(text)
// Fuzz vlozeny komentar
  print(" ")
  print(c)
  print(" ")
  print(d)
  print("\n")

// Fuzz vlozeny komentar
def earlyExit(value: Int): Unit =
  if value > 0 then
// Fuzz vlozeny komentar
    print("positive\n")
    return
  else
    print("other\n")
    ()
// Fuzz vlozeny komentar
  print("after\n")

def returnValue(value: Int): Int = /* inline */
  val next = value + 1
// Fuzz vlozeny komentar
  return next * 2

def returnCall(value: Int): Int =
  val next = value + 1

  return twice(next)


def isEven(n: Int): Int =
  if n == 0 then /* inline */
    print("e") /* inline */
    return 1
  else /* inline */
    val previous = n - 1
    return isOdd(previous)

def isOdd(n: Int): Int =
  if n == 0 then

    print("o")
    return 0
// Fuzz vlozeny komentar
  else
    val previous = n - 1
    return isEven(previous) /* inline */
// Fuzz vlozeny komentar

@main def functionsExample(): Unit =
// Fuzz vlozeny komentar
  val first = constant()

  var second = wrap(first)
// Fuzz vlozeny komentar
  second = add(second, 2)
// Fuzz vlozeny komentar
  print(second)
  print("\n")
  val text = literalBody()
  print(text)
// Fuzz vlozeny komentar

  print("\n")
  discarded()

  val widened = widen(3)
  val widenedText = toStr(widened)
  print(widenedText)
// Fuzz vlozeny komentar
  print("\n")
  val call = lastCall(1)
  print(call)
  print("\n")
  endsWithAssignment()
  endsWithLoop()
  print("\n")
// Fuzz vlozeny komentar
  takesUnit(())
  takesAll(1, 2.5, "three", ())
  takesAll(first, widened, text, ())
// Fuzz vlozeny komentar
  earlyExit(1)
  earlyExit(0) /* inline */
  val fromReturn = returnValue(1)
  val fromReturnCall = returnCall(fromReturn)
  print(fromReturnCall)
  print("\n")
  val parity = isEven(3)
  print(parity)
  print("\n") /* inline */
