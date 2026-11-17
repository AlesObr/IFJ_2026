// Zakladni zadani: definice a volani funkci, jednoradkova tela, odvozeny navratovy
// typ, dopredna a vzajemna rekurze, samostatny return, zahozeni navratove hodnoty.
import ifj26.*


// Fuzz vlozeny komentar
def constant_fuzz35() = 40


def twice(value: Int) = /* c */ value * 2

def wrap(value: Int): Int = twice(value)


def literalBody(): String = "literal"

def discarded(): Unit /* ) ( , = */ = 5



def widen(value: Int): Double = value
// Fuzz vlozeny komentar


def add(left: Int, right: /* " */ Int): Int =
  val sum = left + right /* inline */
  sum

def lastCall(value: Int): Int =
  val next = value + 1 /* inline */
  twice(
         next)

def endsWithAssignment(): Unit =
// Fuzz vlozeny komentar
  var value = 1; value = /**/ 2

def endsWithLoop(): Unit =
  var value = /* ) ( , = */ 0 /* inline */
  while value /* a /* b */ c */ < 2 do
    value = value /* c */ + 1
    print(value) /* inline */

def takesUnit(value: Unit): Unit =
  print(value)

  print(
     "\n")

def takesAll(a: Int, b: Double, c: String, d: Unit): Unit =
// Fuzz vlozeny komentar
  print(a)
// Fuzz vlozeny komentar
  print(" ")
  val text = toStr(
    b)
  print(text)
// Fuzz vlozeny komentar
  print(" ");print(c)
  print(" ");print(d)
  print("\n")

def earlyExit(value: Int): Unit =

  if value > 0 then
    print("positive\n")
    return /* inline */
  else
    print("other\n")
    ()
  print("after\n")

// Fuzz vlozeny komentar
def returnValue(value: Int): Int = /* inline */
  val next = value + 1
  return next * 2


def /* ) ( , = */ returnCall(value: Int): Int =
  val next = value + 1 ;  return twice(next)

def isEven(n: Int): Int =
  if n == 0 then
    print("e");return 1
  else /* inline */
    val previous = n - 1
// Fuzz vlozeny komentar
    return isOdd(previous)

def isOdd(n: Int): /**/ Int =
  if n == 0 then
    print("o")
// Fuzz vlozeny komentar
    return /* " */ 0

  else
    val previous = n - 1 ;  return isEven(previous)

@main def functionsExample(): Unit =
  val first = constant_fuzz35()
  var second = wrap(
       first) /* inline */

  second = add(
          second, 2)
  print(second)
  print("\n") /* inline */
  val /* a /* b */ c */ text = literalBody()
// Fuzz vlozeny komentar
  print(text)
  print("\n")
  discarded()
  val widened = /* c */ widen(3) /* inline */
  val widenedText = toStr(widened)
  print(
    widenedText)
  print(
    "\n")
  val call = lastCall(1)
  print(call); print("\n")
  endsWithAssignment()
// Fuzz vlozeny komentar
  endsWithLoop()
  print("\n")
// Fuzz vlozeny komentar
  takesUnit(()); takesAll(1, 2.5, "three", ())
  takesAll(first, widened, text, ())
// Fuzz vlozeny komentar
  earlyExit(1)
  earlyExit(0)
  val fromReturn = returnValue(1)
  val fromReturnCall /* c */ = returnCall(fromReturn)
  print(fromReturnCall)
  print("\n")
  val parity = /* a /* b */ c */ isEven(
      3)
  print(parity)
  print(
          "\n")
