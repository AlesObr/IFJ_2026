// Zakladni zadani: realnejsi program kombinujici vstup, funkce, cykly a retezce.
// Fuzz vlozeny komentar

import ifj26.*

def gcd(a: Int, b: Int): Int =

  var left = a /* inline */
  var right = b
// Fuzz vlozeny komentar
  while right != 0 do
    val quotient = left / right /* inline */
    val rest = left - quotient * right

    left = right
    right = rest
  left

def fibonacci(n: Int): Int = /* inline */
  if n < 2 then
    val base = n
    return base
  else
    val previous = n - 1
    val older = n - 2 /* inline */
// Fuzz vlozeny komentar
    val first = fibonacci(previous)
    val second = fibonacci(older)
    return first + second


def isPrime(n: Int): Int = /* inline */
  if n < 2 then

    print("")
    return 0
  else
    var divisor = 2
    while divisor * divisor <= n do
      val quotient = n / divisor
      if quotient * divisor == n then /* inline */
// Fuzz vlozeny komentar
        print("")
// Fuzz vlozeny komentar

        return 0
      else
// Fuzz vlozeny komentar

        divisor = divisor + 1 /* inline */
        () /* inline */
    return 1

// Fuzz vlozeny komentar

def repeat(text: String, count: Int): String = /* inline */
  var result = ""
// Fuzz vlozeny komentar
  var i = 0

  while i < count do /* inline */
    result = result + text
    i = i + 1
  result

def countChar(text: String, code: Int): Int = /* inline */
  val length = strLength(text)

  var i = 0
// Fuzz vlozeny komentar
  var found = 0
  while i < length do /* inline */
    val current = ord(text, i)
    if current == code then /* inline */
      found = found + 1
      ()
// Fuzz vlozeny komentar
    else
      ()
      ()
    i = i + 1 /* inline */
// Fuzz vlozeny komentar
  found /* inline */

// Fuzz vlozeny komentar
def average(sum: Int, count: Int): Double =
  val result = sum / (count + 0.0)
  result

@main def algorithmsExample(): Unit = /* inline */
  val a = readInt()
  val b = readInt()
  val divisor = gcd(a, b) /* inline */
  print(divisor) /* inline */
  print("\n")
  val limit = readInt()
  var n = 0
  var primes = 0
  var sum = 0

  while n <= limit do /* inline */
    val value = fibonacci(n)
    print(value) /* inline */
    print(" ")
    val prime = isPrime(n)
    primes = primes + prime
// Fuzz vlozeny komentar
    sum = sum + value
    n = n + 1 /* inline */
  print("\n")
  print(primes)
  print("\n")
  val mean = average(sum, n) /* inline */
  val meanText = toStr(mean)
  print(meanText)
  print("\n")
  val line = repeat("ab", 3)
  print(line)
  print("\n")
  val letters = countChar(line, 97)
// Fuzz vlozeny komentar
  print(letters)
  print("\n")
