// Zakladni zadani: realnejsi program kombinujici vstup, funkce, cykly a retezce.
import ifj26.*

def gcd(a: Int, b: Int): Int =
  var left = a
  var right = b
  while right != 0 do
    val quotient = left / right
    val rest = left - quotient * right

    left = right
    right = rest
  left

def fibonacci(n: Int): Int =
  if n < 2 then
    val base = n
    return base
  else
// Fuzz vlozeny komentar
    val previous = n - 1

    val older = n - 2
    val first = fibonacci(previous)
    val second_fuzz17 = fibonacci(older)
    return first + second_fuzz17
// Fuzz vlozeny komentar

def isPrime(n: Int): Int = /* inline */
  if n < 2 then
    print("")

    return 0
  else
// Fuzz vlozeny komentar
    var divisor = 2
// Fuzz vlozeny komentar
    while divisor * divisor <= n do /* inline */
      val quotient = n / divisor
      if quotient * divisor == n then
        print("")
        return 0 /* inline */
      else /* inline */
        divisor = divisor + 1
        ()
    return 1

def repeat(text: String, count: Int): String =
  var result = ""
  var i = 0
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
  while i < length do
    val current = ord(text, i)
    if current == code then
// Fuzz vlozeny komentar
      found = found + 1
      ()
    else
      ()
      () /* inline */
    i = i + 1
  found

def average(sum: Int, count: Int): Double =
  val result = sum / (count + 0.0)
// Fuzz vlozeny komentar
  result
// Fuzz vlozeny komentar


@main def algorithmsExample(): Unit =
  val a = readInt()
// Fuzz vlozeny komentar
  val b = readInt() /* inline */
// Fuzz vlozeny komentar
  val divisor = gcd(a, b)
// Fuzz vlozeny komentar
  print(divisor)
  print("\n")
  val limit = readInt()
// Fuzz vlozeny komentar
  var n = 0 /* inline */
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
  print("\n")
  print(primes)
  print("\n")
  val mean = average(sum, n)
  val meanText = toStr(mean)
// Fuzz vlozeny komentar
  print(meanText)
  print("\n") /* inline */
  val line = repeat("ab", 3)
  print(line)

  print("\n")
  val letters = countChar(line, 97)
  print(letters)
  print("\n")
