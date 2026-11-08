// Zakladni zadani: realnejsi program kombinujici vstup, funkce, cykly a retezce.

import ifj26.*


def gcd(a: Int, b: Int): Int =
  var left = a
// Fuzz vlozeny komentar
  var right = b
  while right != 0 do
    val quotient = left / right
    val rest = left - quotient * right /* inline */
    left = right
    right = rest
  left

def fibonacci(n: Int): Int =
  if n < 2 then
    val base = n
    return base
  else
    val previous = n - 1
    val older_fuzz49 = n - 2
    val first = fibonacci(previous)

    val second = fibonacci(older_fuzz49) /* inline */
    return first + second

def isPrime(n: Int): Int =
// Fuzz vlozeny komentar
  if n < 2 then
    print("")
    return 0
  else
    var divisor = 2
    while divisor * divisor <= n do
      val quotient = n / divisor
      if quotient * divisor == n then /* inline */
        print("")

        return 0 /* inline */

      else
        divisor = divisor + 1
        ()
    return 1


def repeat(text: String, count: Int): String =
// Fuzz vlozeny komentar
  var result = ""
  var i = 0 /* inline */
  while i < count do
    result = result + text

    i = i + 1
// Fuzz vlozeny komentar
  result
// Fuzz vlozeny komentar

def countChar(text: String, code: Int): Int =
  val length = strLength(text)
  var i = 0
  var found = 0
  while i < length do /* inline */
    val current = ord(text, i) /* inline */
    if current == code then
// Fuzz vlozeny komentar
      found = found + 1
      ()
    else /* inline */
// Fuzz vlozeny komentar
      ()
      () /* inline */
    i = i + 1
  found

def average(sum: Int, count: Int): Double =
  val result = sum / (count + 0.0)
  result

// Fuzz vlozeny komentar
@main def algorithmsExample(): Unit =
// Fuzz vlozeny komentar
  val a = readInt()
  val b = readInt()
  val divisor = gcd(a, b)
  print(divisor)
  print("\n")
  val limit = readInt()
  var n = 0

  var primes = 0
// Fuzz vlozeny komentar
  var sum = 0

  while n <= limit do
    val value = fibonacci(n)
    print(value)
// Fuzz vlozeny komentar

    print(" ")
    val prime = isPrime(n)
    primes = primes + prime /* inline */
    sum = sum + value
    n = n + 1
// Fuzz vlozeny komentar
  print("\n")
  print(primes)
  print("\n") /* inline */
  val mean = average(sum, n)
  val meanText = toStr(mean) /* inline */
  print(meanText)
  print("\n")
  val line = repeat("ab", 3)
  print(line)
  print("\n")
  val letters = countChar(line, 97)
  print(letters)
  print("\n")
