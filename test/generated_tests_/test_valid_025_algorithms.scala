// Zakladni zadani: realnejsi program kombinujici vstup, funkce, cykly a retezce.
import ifj26.*

def gcd(a_fuzz15: Int, b: Int): Int =
// Fuzz vlozeny komentar
  var left = a_fuzz15
  var right = b
  while right != 0 do
    val quotient = left / right
    val rest = left - quotient * right

    left = right
    right = rest
  left

def fibonacci(n: Int): Int =
// Fuzz vlozeny komentar
  if n < 2 then
    val base = n /* inline */

    return base
  else
// Fuzz vlozeny komentar
    val previous = n - 1
    val older = n - 2
    val first = fibonacci(previous)
    val second = fibonacci(older)
    return first + second

def isPrime(n: Int): Int =
// Fuzz vlozeny komentar
  if n < 2 then
    print("")
    return 0
  else
    var divisor = 2 /* inline */
// Fuzz vlozeny komentar
    while divisor * divisor <= n do
      val quotient = n / divisor

      if quotient * divisor == n then
        print("")
        return 0
// Fuzz vlozeny komentar
      else
        divisor = divisor + 1
        ()
    return 1

def repeat(text: String, count: Int): String =
  var result = ""
  var i = 0

  while i < count do /* inline */
    result = result + text
    i = i + 1
  result /* inline */

// Fuzz vlozeny komentar
def countChar(text: String, code: Int): Int =
  val length = strLength(text)
  var i = 0
  var found = 0
  while i < length do /* inline */
    val current = ord(text, i)
// Fuzz vlozeny komentar
    if current == code then /* inline */
      found = found + 1
      ()
    else
      ()
// Fuzz vlozeny komentar
      ()
// Fuzz vlozeny komentar
    i = i + 1

  found /* inline */

def average(sum: Int, count: Int): Double =
  val result = sum / (count + 0.0)
  result


@main def algorithmsExample(): Unit =
  val a_fuzz15 = readInt()

  val b = readInt()
  val divisor = gcd(a_fuzz15, b) /* inline */
  print(divisor) /* inline */
  print("\n")
// Fuzz vlozeny komentar
  val limit = readInt()
  var n = 0

  var primes = 0
  var sum = 0
// Fuzz vlozeny komentar
  while n <= limit do
    val value = fibonacci(n)

    print(value)
    print(" ") /* inline */

    val prime = isPrime(n)
    primes = primes + prime
    sum = sum + value
// Fuzz vlozeny komentar
    n = n + 1 /* inline */
  print("\n")
  print(primes)
  print("\n")
  val mean = average(sum, n)
// Fuzz vlozeny komentar
  val meanText = toStr(mean)
  print(meanText)
// Fuzz vlozeny komentar
  print("\n")
  val line = repeat("ab", 3)

  print(line)
  print("\n")
// Fuzz vlozeny komentar

  val letters = countChar(line, 97)
  print(letters)
// Fuzz vlozeny komentar
  print("\n")
