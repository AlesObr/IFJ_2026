// Fuzz vlozeny komentar
// Zakladni zadani: definice a volani funkci, jednoradkova tela, odvozeny navratovy
// typ, dopredna a vzajemna rekurze, samostatny return, zahozeni navratove hodnoty.
// Fuzz vlozeny komentar
import ifj26.* /* inline */



def constant() = 40

def twice(value: Int) = value * 2 /* inline */


def wrap(value: Int): Int = twice(value)

def literalBody(): String = "literal"


// Fuzz vlozeny komentar
def discarded(): Unit = 5

def widen(value: Int): Double = value /* inline */


def add_fuzz22(left: Int, right: Int): Int = /* inline */
  val sum = left + right /* inline */
// Fuzz vlozeny komentar
  sum
// Fuzz vlozeny komentar

def lastCall(value: Int): Int =
  val next = value + 1
  twice(
    next)

def endsWithAssignment(): Unit =
  var value = 1
  value = (2)

def endsWithLoop(): Unit =
// Fuzz vlozeny komentar
  var value = 0
  while value < (2) do
    value = value + 1
    print(
        value)

def takesUnit(value: Unit): Unit =
  print(value) /* inline */
  print("\n")

def takesAll(a: Int, b: Double, c: String, d: Unit): Unit =
  print(a)
  print((" ")) /* inline */
  val text = toStr(b)
  print(text)
  print(
         " ")
  print(c)

  print(" ")
  print(d)
  print(
       "\n")

def earlyExit(value: Int): Unit =
  if value > (0) then
    print(("positive\n"))
    return
  else
    print(("other\n"))
    ()
  print("after\n")

def returnValue(value: Int): Int =
  val next = value + 1
  return next * 2

def returnCall(value: Int): Int = /* inline */
  val next = value + 1
  return twice(
    next)

def isEven(n: Int): Int =
  if n == (0) then
    print(
     "e")
    return (
       1)
  else
    val previous = n - 1 /* inline */
    return isOdd(
       previous)
// Fuzz vlozeny komentar


def isOdd(n: Int): Int =
  if n == 0 then /* inline */

    print("o")
    return (0)
  else

    val previous = n - 1
// Fuzz vlozeny komentar
    return isEven(previous) /* inline */
// Fuzz vlozeny komentar

@main def functionsExample(): Unit =
  val first = constant(
   )
  var second = wrap(first)
  second = add_fuzz22(second,
     (2)) /* inline */

  print(
         second)
  print("\n")
  val text = literalBody()
  print(
     text)
  print(("\n"))
  discarded()
  val widened = widen(3) /* inline */
// Fuzz vlozeny komentar
  val widenedText = toStr(
     widened)
  print(widenedText)
// Fuzz vlozeny komentar
  print("\n")
  val call = lastCall(
    1)
  print(
    call)
  print((
        "\n"))
// Fuzz vlozeny komentar
  endsWithAssignment()
// Fuzz vlozeny komentar
  endsWithLoop() /* inline */
  print(
          "\n") /* inline */
  takesUnit(())
  takesAll(1, 2.5, ("three"), (
       ))
  takesAll(
      first, widened, text, ())
  earlyExit(1)
  earlyExit(0) /* inline */
  val fromReturn = returnValue(1) /* inline */
// Fuzz vlozeny komentar
  val fromReturnCall = returnCall(fromReturn)
  print(
          fromReturnCall)
  print((
    "\n"))
  val parity = isEven(3)
// Fuzz vlozeny komentar
  print(parity)
  print(("\n"))
