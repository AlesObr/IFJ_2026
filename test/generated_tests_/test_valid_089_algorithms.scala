// Fuzz vlozeny komentar
// Zakladni zadani: realnejsi program kombinujici vstup, funkce, cykly a retezce.
import ifj26.*

// Fuzz vlozeny komentar
def gcd(a: Int, b: Int): Int =
  var left = a
  var right = b
  while right != 0 do /* inline */
// Fuzz vlozeny komentar
    val quotient = left / right
// Fuzz vlozeny komentar
    val rest = left - quotient * right
    left = right
    right = rest
  left /* inline */

def fibonacci_fuzz46(n: Int): Int =
  if n < 2 then /* inline */
    val base = n
    return base
  else
    val previous = n - 1
    val older = n - 2
// Fuzz vlozeny komentar
    val first = fibonacci_fuzz46(previous)
// Fuzz vlozeny komentar
    val second = fibonacci_fuzz46(older) /* inline */
// Fuzz vlozeny komentar
    return first + second

// Fuzz vlozeny komentar
def isPrime(n: Int): Int =
  if n < 2 then /* inline */
// Fuzz vlozeny komentar
    print("")
// Fuzz vlozeny komentar
    return 0
  else
    var divisor = 2
    while divisor * divisor <= n do /* inline */
      val quotient = n / divisor
      if quotient * divisor == n then
        print("")
        return 0
      else /* inline */
        divisor = divisor + 1

        ()
// Fuzz vlozeny komentar
    return 1
// Fuzz vlozeny komentar

def repeat(text: String, count: Int): String =

  var result = ""
  var i = 0
// Fuzz vlozeny komentar
  while i < count do
    result = result + text
    i = i + 1
  result


def countChar(text: String, code: Int): Int =
  val length = strLength(text)
// Fuzz vlozeny komentar

  var i = 0
  var found = 0
  while i < length do

    val current = ord(text, i)
    if current == code then
      found = found + 1 /* inline */
      () /* inline */
    else
// Fuzz vlozeny komentar
      ()
// Fuzz vlozeny komentar
      ()
    i = i + 1
  found
// Fuzz vlozeny komentar


def average(sum: Int, count: Int): Double =
  val result = sum / (count + 0.0)
  result

// Fuzz vlozeny komentar
@main def algorithmsExample(): Unit =
  val a = readInt()
// Fuzz vlozeny komentar
  val b = readInt()
  val divisor = gcd(a, b)
  print(divisor)
  print("\n")
  val limit = readInt()
// Fuzz vlozeny komentar
  var n = 0

  var primes = 0
  var sum = 0 /* inline */
  while n <= limit do
    val value = fibonacci_fuzz46(n)

    print(value)
// Fuzz vlozeny komentar
    print(" ")
    val prime = isPrime(n)
// Fuzz vlozeny komentar
    primes = primes + prime

    sum = sum + value
// Fuzz vlozeny komentar
    n = n + 1 /* inline */
// Fuzz vlozeny komentar
  print("\n") /* inline */
  print(primes) /* inline */
  print("\n")
  val mean = average(sum, n)
  val meanText = toStr(mean)

  print(meanText) /* inline */
  print("\n")
  val line = repeat("ab", 3)
// Fuzz vlozeny komentar
  print(line) /* inline */
  print("\n")
  val letters = countChar(line, 97)
  print(letters)
  print("\n")
