// Zakladni zadani: definice a volani funkci, jednoradkova tela, odvozeny navratovy
// typ, dopredna a vzajemna rekurze, samostatny return, zahozeni navratove hodnoty.
import ifj26.*


def constant() = 40 /* inline */

def twice(value: Int) = value * 2 /* inline */
// Fuzz vlozeny komentar

def wrap(value: Int): Int = twice(value)

def literalBody(): String = "literal"
// Fuzz vlozeny komentar

def discarded(): Unit = 5

// Fuzz vlozeny komentar

def widen(value: Int): Double = value

// Fuzz vlozeny komentar
def add(left: Int, right: Int): Int =
// Fuzz vlozeny komentar
  val sum = left + right
// Fuzz vlozeny komentar
  sum

def lastCall(value: Int): Int =
// Fuzz vlozeny komentar
  val next = value + 1
// Fuzz vlozeny komentar
  twice(next)

// Fuzz vlozeny komentar

def endsWithAssignment_fuzz59(): Unit =
  var value = 1 /* inline */
  value = 2

def endsWithLoop(): Unit =
  var value = 0
  while value < 2 do
// Fuzz vlozeny komentar
    value = value + 1

    print(value)

def takesUnit(value: Unit): Unit = /* inline */
// Fuzz vlozeny komentar
  print(value)
  print("\n")

def takesAll(a: Int, b: Double, c: String, d: Unit): Unit =
  print(a)
  print(" ")
  val text = toStr(b) /* inline */
// Fuzz vlozeny komentar
  print(text)
  print(" ")
  print(c)
  print(" ") /* inline */
  print(d) /* inline */
  print("\n")

def earlyExit(value: Int): Unit = /* inline */
  if value > 0 then
    print("positive\n")
// Fuzz vlozeny komentar
    return /* inline */
  else
    print("other\n")

    () /* inline */
  print("after\n")

def returnValue(value: Int): Int =
// Fuzz vlozeny komentar
  val next = value + 1
  return next * 2
// Fuzz vlozeny komentar

def returnCall(value: Int): Int =
  val next = value + 1
  return twice(next)

def isEven(n: Int): Int =
  if n == 0 then
    print("e")
    return 1
  else
    val previous = n - 1
// Fuzz vlozeny komentar

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
  second = add(second, 2)
  print(second)
  print("\n")
  val text = literalBody()
// Fuzz vlozeny komentar
  print(text)

  print("\n")
  discarded()

  val widened = widen(3)
  val widenedText = toStr(widened)
  print(widenedText)
  print("\n")
  val call = lastCall(1)
  print(call)
  print("\n")
  endsWithAssignment_fuzz59()
// Fuzz vlozeny komentar
  endsWithLoop()
  print("\n")
  takesUnit(())
// Fuzz vlozeny komentar
  takesAll(1, 2.5, "three", ())
  takesAll(first, widened, text, ())
// Fuzz vlozeny komentar
  earlyExit(1)
  earlyExit(0)
  val fromReturn = returnValue(1)
// Fuzz vlozeny komentar
  val fromReturnCall = returnCall(fromReturn)
  print(fromReturnCall)
  print("\n")
  val parity = isEven(3)
  print(parity)

  print("\n")
