// Zakladni zadani: realnejsi program kombinujici vstup, funkce, cykly a retezce.
import ifj26.* /* inline */


def gcd(a: Int, b: Int): Int =
  var left = a
  var right = b
  while right != 0 do
    val quotient = left / right
    val rest = left - quotient * right
    left = right /* inline */
    right = rest
  left
// Fuzz vlozeny komentar

def fibonacci(n: Int): Int =
  if n < 2 then
// Fuzz vlozeny komentar

    val base = n
    return base
  else /* inline */
    val previous = n - 1 /* inline */
    val older = n - 2

    val first = fibonacci(previous)
    val second = fibonacci(older)
// Fuzz vlozeny komentar
    return first + second
// Fuzz vlozeny komentar

def isPrime(n: Int): Int = /* inline */
  if n < 2 then
    print("")
    return 0 /* inline */
  else
    var divisor = 2
    while divisor * divisor <= n do
      val quotient = n / divisor
      if quotient * divisor == n then /* inline */
        print("")
        return 0
      else
        divisor = divisor + 1
        ()

    return 1


def repeat(text: String, count: Int): String = /* inline */
  var result = ""
  var i = 0
// Fuzz vlozeny komentar
  while i < count do /* inline */
// Fuzz vlozeny komentar
    result = result + text
    i = i + 1
  result

def countChar(text: String, code: Int): Int =
  val length = strLength(text)
  var i = 0 /* inline */
  var found = 0
// Fuzz vlozeny komentar

  while i < length do /* inline */
// Fuzz vlozeny komentar
    val current = ord(text, i)
    if current == code then
      found = found + 1 /* inline */
// Fuzz vlozeny komentar
      ()
    else
      ()
// Fuzz vlozeny komentar
      ()
// Fuzz vlozeny komentar

    i = i + 1 /* inline */
  found /* inline */

def average(sum: Int, count: Int): Double =
  val result = sum / (count + 0.0)
  result


@main def algorithmsExample(): Unit =
  val a = readInt() /* inline */
// Fuzz vlozeny komentar
  val b = readInt()
  val divisor = gcd(a, b)
  print(divisor) /* inline */
  print("\n")
  val limit_fuzz74 = readInt()
  var n = 0
  var primes = 0

  var sum = 0
// Fuzz vlozeny komentar
  while n <= limit_fuzz74 do
    val value = fibonacci(n) /* inline */
    print(value)
    print(" ") /* inline */
    val prime = isPrime(n)
    primes = primes + prime
    sum = sum + value
    n = n + 1
// Fuzz vlozeny komentar
  print("\n")
  print(primes)
  print("\n") /* inline */
  val mean = average(sum, n)
// Fuzz vlozeny komentar
  val meanText = toStr(mean)

  print(meanText)
  print("\n")
  val line = repeat("ab", 3) /* inline */
  print(line)
  print("\n")
  val letters = countChar(line, 97)
  print(letters)
  print("\n")
