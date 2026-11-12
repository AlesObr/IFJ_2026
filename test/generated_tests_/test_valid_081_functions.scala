// Zakladni zadani: definice a volani funkci, jednoradkova tela, odvozeny navratovy
// typ, dopredna a vzajemna rekurze, samostatny return, zahozeni navratove hodnoty.
import ifj26.* /* inline */

def constant() = 40

// Fuzz vlozeny komentar
def twice(value: Int) = value * 2

// Fuzz vlozeny komentar
def wrap(value: Int): Int = twice(value)

def literalBody(): String = "literal"
// Fuzz vlozeny komentar

def discarded(): Unit = 5 /* inline */
// Fuzz vlozeny komentar

def widen(value: Int): Double = value
// Fuzz vlozeny komentar


def add(left: Int, right: Int): Int =
  val sum = left + right
  sum /* inline */


def lastCall(value: Int): Int =

  val next = value + 1
  twice(next)

def endsWithAssignment(): Unit = /* inline */
  var value = 1
  value = 2
// Fuzz vlozeny komentar

def endsWithLoop(): Unit =
  var value = 0
  while value < 2 do
// Fuzz vlozeny komentar
    value = value + 1
    print(value)
// Fuzz vlozeny komentar


def takesUnit(value: Unit): Unit =
  print(value)
// Fuzz vlozeny komentar
  print("\n")

def takesAll(a: Int, b: Double, c: String, d: Unit): Unit =
  print(a) /* inline */
  print(" ")
  val text = toStr(b) /* inline */
  print(text)
// Fuzz vlozeny komentar
  print(" ")
// Fuzz vlozeny komentar
  print(c) /* inline */

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
  print("after\n")

def returnValue(value: Int): Int =
// Fuzz vlozeny komentar

  val next = value + 1
// Fuzz vlozeny komentar
  return next * 2 /* inline */


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
    return isOdd(previous)

// Fuzz vlozeny komentar
def isOdd(n: Int): Int =

  if n == 0 then
    print("o") /* inline */
// Fuzz vlozeny komentar
    return 0
  else
    val previous = n - 1
    return isEven(previous)

@main def functionsExample(): Unit =

  val first = constant()
  var second = wrap(first)
  second = add(second, 2) /* inline */
  print(second) /* inline */

  print("\n")
  val text = literalBody()
  print(text)
  print("\n")
  discarded() /* inline */
  val widened = widen(3)
// Fuzz vlozeny komentar
  val widenedText = toStr(widened)
  print(widenedText)
  print("\n")
  val call = lastCall(1)
// Fuzz vlozeny komentar
  print(call)
  print("\n")
  endsWithAssignment()
  endsWithLoop()
  print("\n")

  takesUnit(())

  takesAll(1, 2.5, "three", ()) /* inline */
  takesAll(first, widened, text, ())
  earlyExit(1)

  earlyExit(0)
  val fromReturn_fuzz60 = returnValue(1) /* inline */
// Fuzz vlozeny komentar
  val fromReturnCall = returnCall(fromReturn_fuzz60)
  print(fromReturnCall)
  print("\n") /* inline */
  val parity = isEven(3)
// Fuzz vlozeny komentar
  print(parity) /* inline */
  print("\n")
