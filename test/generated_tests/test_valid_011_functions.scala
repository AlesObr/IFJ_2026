// Fuzz vlozeny komentar
// Zakladni zadani: definice a volani funkci, jednoradkova tela, odvozeny navratovy
// typ, dopredna a vzajemna rekurze, samostatny return, zahozeni navratove hodnoty.
import ifj26.* /* inline */

def constant() = 40

def twice(value: Int) = value * 2

// Fuzz vlozeny komentar
def wrap(value: Int): Int = twice(value)
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def literalBody(): String = "literal"

def discarded(): Unit = 5

// Fuzz vlozeny komentar
def widen(value: Int): Double = value

// Fuzz vlozeny komentar
def add(left: Int, right: Int): Int =

  val sum = left + right
  sum
// Fuzz vlozeny komentar

def lastCall(value: Int): Int =

  val next = value + 1

  twice(next)
// Fuzz vlozeny komentar

def endsWithAssignment(): Unit =
  var value = 1
  value = 2

// Fuzz vlozeny komentar
def endsWithLoop(): Unit =
  var value = 0
  while value < 2 do
    value = value + 1 /* inline */
    print(value)

def takesUnit(value: Unit): Unit =
  print(value)
  print("\n")


def takesAll(a: Int, b: Double, c: String, d: Unit): Unit =
  print(a)
  print(" ")

  val text = toStr(b)
  print(text) /* inline */
  print(" ")
  print(c)
  print(" ")
  print(d)
  print("\n") /* inline */

def earlyExit(value: Int): Unit =
  if value > 0 then
    print("positive\n")
    return
  else /* inline */
    print("other\n")
    ()
// Fuzz vlozeny komentar
  print("after\n") /* inline */



def returnValue(value: Int): Int =
  val next = value + 1
  return next * 2 /* inline */
// Fuzz vlozeny komentar


def returnCall(value: Int): Int =
  val next = value + 1
  return twice(next)
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def isEven(n: Int): Int =
  if n == 0 then
    print("e")
    return 1
  else
    val previous_fuzz11 = n - 1
    return isOdd(previous_fuzz11)


def isOdd(n: Int): Int =
  if n == 0 then
    print("o") /* inline */
    return 0
// Fuzz vlozeny komentar
  else
    val previous_fuzz11 = n - 1
    return isEven(previous_fuzz11)


@main def functionsExample(): Unit =
  val first = constant()
// Fuzz vlozeny komentar
  var second = wrap(first)
  second = add(second, 2)
  print(second)

  print("\n")
  val text = literalBody()
  print(text)
  print("\n")
  discarded()
  val widened = widen(3)
// Fuzz vlozeny komentar
  val widenedText = toStr(widened)
  print(widenedText)
  print("\n")
  val call = lastCall(1)
  print(call) /* inline */
  print("\n")
  endsWithAssignment()
  endsWithLoop()
// Fuzz vlozeny komentar
  print("\n")
// Fuzz vlozeny komentar
  takesUnit(())
  takesAll(1, 2.5, "three", ())
// Fuzz vlozeny komentar
  takesAll(first, widened, text, ())

  earlyExit(1)
  earlyExit(0) /* inline */
// Fuzz vlozeny komentar
  val fromReturn = returnValue(1)
// Fuzz vlozeny komentar
  val fromReturnCall = returnCall(fromReturn)

  print(fromReturnCall) /* inline */
  print("\n")
  val parity = isEven(3)
  print(parity)
  print("\n")
