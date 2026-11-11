// Zakladni zadani: realnejsi program kombinujici vstup, funkce, cykly a retezce.
import ifj26.*

// Fuzz vlozeny komentar

def gcd(a: Int, b: Int): Int =

  var left = a
  var right = b
  while right != 0 do
    val quotient = left / right
// Fuzz vlozeny komentar
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
    val older = n - 2
    val first = fibonacci(previous)
    val second = fibonacci(older)
    return first + second

def isPrime(n: Int): Int =
  if n < 2 then /* inline */
    print("")
    return 0 /* inline */
  else /* inline */
    var divisor = 2
    while divisor * divisor <= n do
      val quotient = n / divisor
// Fuzz vlozeny komentar
      if quotient * divisor == n then
        print("") /* inline */
        return 0 /* inline */
      else
// Fuzz vlozeny komentar
        divisor = divisor + 1
        ()
    return 1

def repeat(text: String, count: Int): String =
  var result = ""
  var i = 0
  while i < count do /* inline */
    result = result + text
    i = i + 1 /* inline */
  result

def countChar(text: String, code: Int): Int =
// Fuzz vlozeny komentar
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
    i = i + 1
  found

def average(sum: Int, count: Int): Double =
  val result = sum / (count + 0.0)
// Fuzz vlozeny komentar
  result
// Fuzz vlozeny komentar

@main def algorithmsExample(): Unit =
  val a = readInt()
  val b = readInt()
  val divisor = gcd(a, b) /* inline */
  print(divisor)
  print("\n")
  val limit = readInt()
  var n = 0
  var primes = 0

  var sum = 0
  while n <= limit do
    val value = fibonacci(n)
    print(value)
    print(" ") /* inline */
    val prime = isPrime(n)

    primes = primes + prime
    sum = sum + value
    n = n + 1
// Fuzz vlozeny komentar
  print("\n") /* inline */
// Fuzz vlozeny komentar
  print(primes)

  print("\n")
  val mean = average(sum, n)
  val meanText = toStr(mean)
  print(meanText)
  print("\n")
// Fuzz vlozeny komentar
  val line = repeat("ab", 3)
  print(line) /* inline */
  print("\n")
  val letters = countChar(line, 97)
  print(letters)
  print("\n")
