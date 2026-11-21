
// Zakladni zadani: realnejsi program kombinujici vstup, funkce, cykly a retezce.
import ifj26.*

// Fuzz vlozeny komentar
def gcd(a: Int, b: Int): Int =
// Fuzz vlozeny komentar
  var left = a /* inline */
  var right = b /* inline */
  while right != 0 do

    val quotient = left / right

    val rest = left - quotient * right
    left = right
// Fuzz vlozeny komentar
    right = rest
  left

def fibonacci(n: Int): Int = /* inline */
  if n < 2 then
    val base = n
// Fuzz vlozeny komentar
    return base /* inline */
// Fuzz vlozeny komentar
  else
// Fuzz vlozeny komentar
    val previous = n - 1
    val older = n - 2
    val first = fibonacci(previous) /* inline */

    val second = fibonacci(older)
    return first + second

def isPrime(n: Int): Int =
  if n < 2 then /* inline */
    print("") /* inline */
    return 0 /* inline */
  else
    var divisor = 2
    while divisor * divisor <= n do
      val quotient = n / divisor
      if quotient * divisor == n then
        print("")

        return 0 /* inline */
      else
        divisor = divisor + 1 /* inline */
        ()
// Fuzz vlozeny komentar
    return 1


def repeat(text: String, count: Int): String =
  var result = ""
  var i = 0
  while i < count do /* inline */
    result = result + text
// Fuzz vlozeny komentar
    i = i + 1
  result


// Fuzz vlozeny komentar
def countChar(text: String, code: Int): Int =
  val length = strLength(text)
  var i = 0
  var found = 0
  while i < length do
// Fuzz vlozeny komentar
    val current = ord(text, i)
    if current == code then
      found = found + 1

      ()
    else
      ()
      ()
    i = i + 1 /* inline */
  found

def average(sum: Int, count: Int): Double =
  val result = sum / (count + 0.0)
// Fuzz vlozeny komentar
  result
// Fuzz vlozeny komentar

@main def algorithmsExample(): Unit =
// Fuzz vlozeny komentar
  val a = readInt()
// Fuzz vlozeny komentar
  val b = readInt()
  val divisor = gcd(a, b)
  print(divisor)
// Fuzz vlozeny komentar
  print("\n")

  val limit = readInt()
// Fuzz vlozeny komentar
  var n = 0 /* inline */
  var primes = 0
  var sum = 0
  while n <= limit do
    val value = fibonacci(n)
// Fuzz vlozeny komentar
    print(value) /* inline */
    print(" ")
    val prime = isPrime(n)
    primes = primes + prime

    sum = sum + value
    n = n + 1
  print("\n")
  print(primes) /* inline */
// Fuzz vlozeny komentar
  print("\n")
// Fuzz vlozeny komentar
  val mean = average(sum, n)
  val meanText = toStr(mean)
  print(meanText)
  print("\n")
  val line = repeat("ab", 3)
  print(line)
  print("\n")
// Fuzz vlozeny komentar

  val letters = countChar(line, 97) /* inline */
  print(letters)
  print("\n")
