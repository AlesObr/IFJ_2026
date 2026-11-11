// Fuzz vlozeny komentar
// Zakladni zadani: realnejsi program kombinujici vstup, funkce, cykly a retezce.
import ifj26.*

def gcd(a: Int, b: Int): Int =
  var left = a
  var right = b
  while right != 0 do
    val quotient = left / right
    val rest = left - quotient * right
    left = right /* inline */
    right = rest
  left

def fibonacci(n: Int): Int =
  if n < 2 then
    val base = n
    return base
  else
    val previous = n - 1
    val older = n - 2 /* inline */
    val first = fibonacci(previous) /* inline */
    val second = fibonacci(older)
    return first + second

def isPrime(n: Int): Int =
// Fuzz vlozeny komentar
  if n < 2 then /* inline */
    print("")
    return 0
// Fuzz vlozeny komentar
  else
    var divisor = 2
    while divisor * divisor <= n do
      val quotient = n / divisor
// Fuzz vlozeny komentar
      if quotient * divisor == n then
        print("")
        return 0
      else
        divisor = divisor + 1
        ()
    return 1 /* inline */

def repeat(text: String, count: Int): String =
  var result = ""
// Fuzz vlozeny komentar
  var i = 0
  while i < count do /* inline */
    result = result + text
    i = i + 1 /* inline */
// Fuzz vlozeny komentar

  result

// Fuzz vlozeny komentar
def countChar(text: String, code: Int): Int = /* inline */
  val length = strLength(text)
  var i = 0
  var found = 0
  while i < length do
    val current = ord(text, i)
// Fuzz vlozeny komentar
    if current == code then
      found = found + 1
      ()
    else
      ()
// Fuzz vlozeny komentar
      ()
    i = i + 1
  found
// Fuzz vlozeny komentar

def average(sum: Int, count: Int): Double =

  val result = sum / (count + 0.0)
  result /* inline */
// Fuzz vlozeny komentar


@main def algorithmsExample(): Unit = /* inline */
  val a = readInt()
  val b = readInt()
  val divisor = gcd(a, b)
  print(divisor)
  print("\n")
  val limit = readInt()
  var n = 0
  var primes = 0
  var sum = 0
  while n <= limit do
    val value = fibonacci(n)
    print(value)
// Fuzz vlozeny komentar
    print(" ")
    val prime = isPrime(n)

    primes = primes + prime
    sum = sum + value
    n = n + 1
  print("\n")
  print(primes)
  print("\n")
  val mean = average(sum, n)
  val meanText = toStr(mean)
  print(meanText)
  print("\n")

  val line = repeat("ab", 3)
  print(line)
  print("\n")
  val letters = countChar(line, 97)
  print(letters) /* inline */
  print("\n")
