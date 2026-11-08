// Fuzz vlozeny komentar
// Zakladni zadani: definice a volani funkci, jednoradkova tela, odvozeny navratovy
// typ, dopredna a vzajemna rekurze, samostatny return, zahozeni navratove hodnoty.
import ifj26.*

// Fuzz vlozeny komentar

def constant() = 40 /* inline */

def twice(value: Int) = value * 2

// Fuzz vlozeny komentar
def wrap(value: Int): Int = twice(value)

def literalBody(): String = "literal"

def discarded(): Unit = 5

def widen(value: Int): Double = value /* inline */

def add(left: Int, right: Int): Int =
  val sum = left + right
  sum

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
// Fuzz vlozeny komentar
    value = value + 1
    print(value)

def takesUnit(value: Unit): Unit = /* inline */
  print(value)
  print("\n")

def takesAll(a: Int, b: Double, c: String, d: Unit): Unit =
// Fuzz vlozeny komentar
  print(a)
  print(" ")
  val text = toStr(b)
  print(text)
  print(" ") /* inline */
  print(c)
  print(" ")
  print(d)
  print("\n")

def earlyExit(value: Int): Unit =
  if value > 0 then
// Fuzz vlozeny komentar
    print("positive\n")
    return
  else /* inline */
// Fuzz vlozeny komentar
    print("other\n")
    ()
  print("after\n")


def returnValue(value: Int): Int =
  val next = value + 1
// Fuzz vlozeny komentar
  return next * 2

def returnCall(value: Int): Int =

  val next = value + 1
  return twice(next)

def isEven(n: Int): Int =

  if n == 0 then
    print("e")
    return 1
  else
    val previous = n - 1

    return isOdd(previous) /* inline */

// Fuzz vlozeny komentar
def isOdd(n: Int): Int =
  if n == 0 then
    print("o")
// Fuzz vlozeny komentar
    return 0
  else /* inline */
    val previous = n - 1 /* inline */
    return isEven(previous) /* inline */

@main def functionsExample(): Unit =
  val first = constant()
// Fuzz vlozeny komentar

  var second = wrap(first)
  second = add(second, 2) /* inline */
  print(second)
  print("\n")
  val text = literalBody()

  print(text)
  print("\n")
  discarded()
// Fuzz vlozeny komentar
  val widened = widen(3)
  val widenedText = toStr(widened) /* inline */
  print(widenedText) /* inline */

  print("\n")
  val call = lastCall(1)
  print(call)

  print("\n")
  endsWithAssignment()
  endsWithLoop() /* inline */

  print("\n")
  takesUnit(())
  takesAll(1, 2.5, "three", ())
  takesAll(first, widened, text, ())
// Fuzz vlozeny komentar

  earlyExit(1)
  earlyExit(0)
// Fuzz vlozeny komentar
  val fromReturn = returnValue(1)
  val fromReturnCall = returnCall(fromReturn)
  print(fromReturnCall) /* inline */
  print("\n")
  val parity = isEven(3)
  print(parity)
  print("\n")
