// Fuzz vlozeny komentar
// Zakladni zadani: realnejsi program kombinujici vstup, funkce, cykly a retezce.
import ifj26.* /* inline */
// Fuzz vlozeny komentar

def gcd(a: Int, b: Int): Int =
  var left = a /* inline */
  var right = b
  while right != 0 do
    val quotient = left / right

    val rest = left - quotient * right
    left = right /* inline */
    right = rest /* inline */
// Fuzz vlozeny komentar
  left

def fibonacci(n: Int): Int =
  if n < 2 then
// Fuzz vlozeny komentar
    val base = n /* inline */
    return base
  else /* inline */

    val previous = n - 1
    val older = n - 2
    val first = fibonacci(previous)

    val second = fibonacci(older) /* inline */
// Fuzz vlozeny komentar
    return first + second

def isPrime(n: Int): Int =
  if n < 2 then
    print("") /* inline */
    return 0
  else
    var divisor = 2
    while divisor * divisor <= n do
      val quotient = n / divisor
      if quotient * divisor == n then
        print("") /* inline */
        return 0
      else
        divisor = divisor + 1
        ()
    return 1

def repeat(text: String, count: Int): String =
  var result = ""
  var i = 0 /* inline */
  while i < count do
    result = result + text
    i = i + 1
  result

def countChar(text: String, code: Int): Int =
  val length = strLength(text)

  var i = 0
  var found = 0
  while i < length do
    val current = ord(text, i)
    if current == code then
      found = found + 1 /* inline */
      ()
    else
      ()
      ()
// Fuzz vlozeny komentar

    i = i + 1
  found

// Fuzz vlozeny komentar
def average(sum: Int, count: Int): Double =
// Fuzz vlozeny komentar

  val result = sum / (count + 0.0)
// Fuzz vlozeny komentar
  result


@main def algorithmsExample(): Unit =
  val a = readInt()
  val b = readInt()
  val divisor = gcd(a, b)
  print(divisor)
// Fuzz vlozeny komentar
  print("\n")
// Fuzz vlozeny komentar
  val limit = readInt()
  var n = 0
// Fuzz vlozeny komentar
  var primes = 0
  var sum = 0
  while n <= limit do
    val value = fibonacci(n)
// Fuzz vlozeny komentar
    print(value) /* inline */

    print(" ")
// Fuzz vlozeny komentar
    val prime = isPrime(n)
    primes = primes + prime
    sum = sum + value
    n = n + 1
// Fuzz vlozeny komentar

  print("\n")

  print(primes)
  print("\n")
  val mean = average(sum, n)
  val meanText = toStr(mean)
  print(meanText) /* inline */
  print("\n")
  val line = repeat("ab", 3) /* inline */
  print(line)
  print("\n") /* inline */
// Fuzz vlozeny komentar
  val letters = countChar(line, 97) /* inline */
  print(letters)
// Fuzz vlozeny komentar
  print("\n")
