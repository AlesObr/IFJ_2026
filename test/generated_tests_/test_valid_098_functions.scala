// Zakladni zadani: definice a volani funkci, jednoradkova tela, odvozeny navratovy
// Fuzz vlozeny komentar

// typ, dopredna a vzajemna rekurze, samostatny return, zahozeni navratove hodnoty.
import ifj26.*

def constant() = 40

// Fuzz vlozeny komentar
def twice(value: Int) = value * 2 /* inline */
// Fuzz vlozeny komentar


def wrap(value: Int): Int = twice(value)

def literalBody(): String = "literal"

def discarded(): Unit = 5

// Fuzz vlozeny komentar
def widen(value: Int): Double = value

def add(left: Int, right: Int): Int =
  val sum = left + right
  sum


def lastCall(value: Int): Int =
  val next = value + 1
  twice(next)
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def endsWithAssignment(): Unit = /* inline */
  var value = 1

  value = 2


def endsWithLoop(): Unit = /* inline */

  var value = 0
  while value < 2 do
// Fuzz vlozeny komentar
    value = value + 1
    print(value)

def takesUnit(value: Unit): Unit =
// Fuzz vlozeny komentar
  print(value) /* inline */
  print("\n")

// Fuzz vlozeny komentar
def takesAll(a: Int, b: Double, c: String, d: Unit): Unit = /* inline */
  print(a)
// Fuzz vlozeny komentar
  print(" ")
  val text = toStr(b) /* inline */
  print(text)
// Fuzz vlozeny komentar
  print(" ")

  print(c)
// Fuzz vlozeny komentar
  print(" ")
  print(d)
  print("\n")


def earlyExit(value: Int): Unit =
  if value > 0 then
    print("positive\n")
    return
  else
    print("other\n")
    ()
  print("after\n")

// Fuzz vlozeny komentar
def returnValue_fuzz87(value: Int): Int =
  val next = value + 1
  return next * 2

def returnCall(value: Int): Int =
  val next = value + 1
  return twice(next)

def isEven(n: Int): Int =
// Fuzz vlozeny komentar
  if n == 0 then
    print("e")
    return 1
  else /* inline */
    val previous = n - 1

    return isOdd(previous)

def isOdd(n: Int): Int =
  if n == 0 then
    print("o")
    return 0 /* inline */
  else
    val previous = n - 1
    return isEven(previous)

@main def functionsExample(): Unit =
  val first = constant()
  var second = wrap(first)
// Fuzz vlozeny komentar
  second = add(second, 2)
// Fuzz vlozeny komentar
  print(second) /* inline */
  print("\n")
  val text = literalBody()
  print(text)
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
// Fuzz vlozeny komentar
  endsWithAssignment()
  endsWithLoop()
  print("\n")
  takesUnit(())
  takesAll(1, 2.5, "three", ())
// Fuzz vlozeny komentar
  takesAll(first, widened, text, ())
// Fuzz vlozeny komentar
  earlyExit(1)
// Fuzz vlozeny komentar

  earlyExit(0) /* inline */
  val fromReturn = returnValue_fuzz87(1)
  val fromReturnCall = returnCall(fromReturn)
// Fuzz vlozeny komentar
  print(fromReturnCall)
  print("\n") /* inline */
  val parity = isEven(3)
  print(parity) /* inline */
  print("\n")
