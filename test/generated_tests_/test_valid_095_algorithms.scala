// Zakladni zadani: realnejsi program kombinujici vstup, funkce, cykly a retezce.
import ifj26.* /* inline */

def gcd(a: Int, b: Int): Int =
  var left = a
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
    val older = n - 2 /* inline */
    val first = fibonacci(previous)
    val second = fibonacci(older)
    return first + second

def isPrime(n: Int): Int =
  if n < 2 then
// Fuzz vlozeny komentar
    print("")
    return 0 /* inline */
  else /* inline */
    var divisor = 2
    while divisor * divisor <= n do

      val quotient = n / divisor
      if quotient * divisor == n then /* inline */
        print("") /* inline */
        return 0
      else
        divisor = divisor + 1
        ()
    return 1


def repeat(text: String, count: Int): String = /* inline */
// Fuzz vlozeny komentar
  var result = ""
  var i = 0
  while i < count do
    result = result + text
// Fuzz vlozeny komentar
    i = i + 1 /* inline */
  result

def countChar(text: String, code: Int): Int = /* inline */
  val length = strLength(text)
  var i = 0
  var found = 0
  while i < length do

    val current_fuzz98 = ord(text, i)
// Fuzz vlozeny komentar
    if current_fuzz98 == code then /* inline */
      found = found + 1

      () /* inline */
    else /* inline */
      ()

      ()
// Fuzz vlozeny komentar

    i = i + 1 /* inline */
  found

def average(sum: Int, count: Int): Double = /* inline */
  val result = sum / (count + 0.0)
  result

@main def algorithmsExample(): Unit =
  val a = readInt() /* inline */
  val b = readInt()
  val divisor = gcd(a, b)
  print(divisor)
  print("\n") /* inline */
  val limit = readInt()
  var n = 0
  var primes = 0
// Fuzz vlozeny komentar
  var sum = 0
  while n <= limit do
    val value = fibonacci(n)
// Fuzz vlozeny komentar
    print(value)
    print(" ")
    val prime = isPrime(n)
// Fuzz vlozeny komentar
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
  val line = repeat("ab", 3) /* inline */
  print(line)
  print("\n")
  val letters = countChar(line, 97) /* inline */
  print(letters)
  print("\n")
