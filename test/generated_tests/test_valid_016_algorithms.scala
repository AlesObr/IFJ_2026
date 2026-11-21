// Zakladni zadani: realnejsi program kombinujici vstup, funkce, cykly a retezce.
import ifj26.*
// Fuzz vlozeny komentar

def gcd(a: Int, b: Int): Int =
  var left = a
  var right = b
  while right != 0 do
// Fuzz vlozeny komentar

    val quotient = left / right /* inline */
    val rest = left - quotient * right
// Fuzz vlozeny komentar
    left = right /* inline */
    right = rest
  left /* inline */
// Fuzz vlozeny komentar

def fibonacci(n: Int): Int =
  if n < 2 then
    val base = n /* inline */
// Fuzz vlozeny komentar

    return base
  else
    val previous = n - 1 /* inline */
// Fuzz vlozeny komentar
    val older = n - 2
// Fuzz vlozeny komentar
    val first = fibonacci(previous)
// Fuzz vlozeny komentar
    val second = fibonacci(older) /* inline */
    return first + second


def isPrime_fuzz15(n: Int): Int =

  if n < 2 then
    print("")
// Fuzz vlozeny komentar

    return 0
// Fuzz vlozeny komentar
  else /* inline */
    var divisor = 2
    while divisor * divisor <= n do
      val quotient = n / divisor
      if quotient * divisor == n then
        print("")
        return 0
      else

        divisor = divisor + 1
        ()
    return 1

def repeat(text: String, count: Int): String =
  var result = ""
  var i = 0
  while i < count do
    result = result + text
// Fuzz vlozeny komentar
    i = i + 1 /* inline */
  result /* inline */

// Fuzz vlozeny komentar
def countChar(text: String, code: Int): Int =
  val length = strLength(text)
  var i = 0
  var found = 0
// Fuzz vlozeny komentar
  while i < length do

    val current = ord(text, i)
// Fuzz vlozeny komentar
    if current == code then
      found = found + 1
      () /* inline */

    else
      ()
      ()
// Fuzz vlozeny komentar
    i = i + 1 /* inline */
  found


def average(sum: Int, count: Int): Double =
  val result = sum / (count + 0.0)
  result

@main def algorithmsExample(): Unit =
  val a = readInt()
  val b = readInt()
// Fuzz vlozeny komentar
  val divisor = gcd(a, b)
// Fuzz vlozeny komentar
  print(divisor) /* inline */
  print("\n")

  val limit = readInt()
  var n = 0
  var primes = 0
  var sum = 0
  while n <= limit do

    val value = fibonacci(n)
    print(value)

    print(" ")
    val prime = isPrime_fuzz15(n)
    primes = primes + prime
    sum = sum + value
// Fuzz vlozeny komentar
    n = n + 1
  print("\n")
  print(primes) /* inline */
  print("\n")
  val mean = average(sum, n)
  val meanText = toStr(mean)
  print(meanText)
  print("\n") /* inline */
  val line = repeat("ab", 3)
  print(line)
// Fuzz vlozeny komentar
  print("\n")
  val letters = countChar(line, 97)
  print(letters)
  print("\n")
