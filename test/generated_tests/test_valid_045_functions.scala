// Zakladni zadani: definice a volani funkci, jednoradkova tela, odvozeny navratovy
// typ, dopredna a vzajemna rekurze, samostatny return, zahozeni navratove hodnoty.
import ifj26.*

def constant() = 40


def twice(value: Int) = value * 2
// Fuzz vlozeny komentar

def wrap(value: Int): Int = twice(value) /* inline */

def literalBody(): String = "literal"

def discarded(): Unit = 5 /* inline */

// Fuzz vlozeny komentar
def widen_fuzz63(value: Int): Double = value /* inline */

def add(left: Int, right: Int): Int =
  val sum = left + right
  sum /* inline */

def lastCall(value: Int): Int =
// Fuzz vlozeny komentar
  val next = value + 1
// Fuzz vlozeny komentar
  twice(next)

def endsWithAssignment(): Unit =
  var value = 1
  value = 2

def endsWithLoop(): Unit =
  var value = 0
// Fuzz vlozeny komentar

  while value < 2 do
// Fuzz vlozeny komentar
    value = value + 1
    print(value)

def takesUnit(value: Unit): Unit =
// Fuzz vlozeny komentar
  print(value) /* inline */
  print("\n")

def takesAll(a: Int, b: Double, c: String, d: Unit): Unit =
  print(a)
  print(" ")
// Fuzz vlozeny komentar
  val text = toStr(b)
// Fuzz vlozeny komentar
  print(text)
// Fuzz vlozeny komentar

  print(" ")
  print(c)
  print(" ")
// Fuzz vlozeny komentar
  print(d)
  print("\n")
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def earlyExit(value: Int): Unit =
  if value > 0 then
    print("positive\n") /* inline */
    return
  else
// Fuzz vlozeny komentar
    print("other\n")
// Fuzz vlozeny komentar

    ()
  print("after\n")
// Fuzz vlozeny komentar

def returnValue(value: Int): Int = /* inline */
  val next = value + 1
// Fuzz vlozeny komentar
  return next * 2 /* inline */

def returnCall(value: Int): Int =

  val next = value + 1
// Fuzz vlozeny komentar
  return twice(next)

def isEven(n: Int): Int = /* inline */
  if n == 0 then

    print("e")
    return 1

  else
    val previous = n - 1
// Fuzz vlozeny komentar
    return isOdd(previous)
// Fuzz vlozeny komentar

def isOdd(n: Int): Int = /* inline */
  if n == 0 then /* inline */

    print("o")
    return 0
  else
    val previous = n - 1
// Fuzz vlozeny komentar
    return isEven(previous)

@main def functionsExample(): Unit =
  val first = constant()
  var second = wrap(first)
  second = add(second, 2)
  print(second)
// Fuzz vlozeny komentar
  print("\n")
// Fuzz vlozeny komentar
  val text = literalBody() /* inline */
  print(text)
  print("\n")
  discarded() /* inline */
  val widened = widen_fuzz63(3)
  val widenedText = toStr(widened) /* inline */
  print(widenedText) /* inline */
  print("\n")

  val call = lastCall(1)
  print(call)
// Fuzz vlozeny komentar
  print("\n")
  endsWithAssignment()
  endsWithLoop()
  print("\n")
// Fuzz vlozeny komentar
  takesUnit(()) /* inline */
// Fuzz vlozeny komentar
  takesAll(1, 2.5, "three", ())
// Fuzz vlozeny komentar
  takesAll(first, widened, text, ())
  earlyExit(1)
  earlyExit(0)
  val fromReturn = returnValue(1)
  val fromReturnCall = returnCall(fromReturn)
  print(fromReturnCall)
  print("\n")
  val parity = isEven(3)
  print(parity)
  print("\n") /* inline */
