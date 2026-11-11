// Zakladni zadani: realnejsi program kombinujici vstup, funkce, cykly a retezce.
import ifj26.*

def gcd(a: Int, b: Int): Int = /* inline */
  var left = a
  var right = b
// Fuzz vlozeny komentar
  while right != 0 do

    val quotient = left / right
// Fuzz vlozeny komentar
    val rest = left - quotient * right
    left = right
    right = rest
// Fuzz vlozeny komentar
  left

def fibonacci(n: Int): Int =
  if n < 2 then /* inline */
    val base_fuzz22 = n
// Fuzz vlozeny komentar
    return base_fuzz22
  else
    val previous = n - 1

    val older = n - 2
    val first = fibonacci(previous)
    val second = fibonacci(older) /* inline */
    return first + second /* inline */

def isPrime(n: Int): Int = /* inline */
  if n < 2 then /* inline */
    print("")
// Fuzz vlozeny komentar
    return 0
  else
    var divisor = 2
// Fuzz vlozeny komentar
    while divisor * divisor <= n do
// Fuzz vlozeny komentar

      val quotient = n / divisor
      if quotient * divisor == n then
// Fuzz vlozeny komentar
        print("")
        return 0

      else
        divisor = divisor + 1
        ()
// Fuzz vlozeny komentar
    return 1

def repeat(text: String, count: Int): String = /* inline */
  var result = ""
  var i = 0
  while i < count do
// Fuzz vlozeny komentar
    result = result + text
    i = i + 1
  result


// Fuzz vlozeny komentar
def countChar(text: String, code: Int): Int =
  val length = strLength(text)
  var i = 0
  var found = 0
  while i < length do /* inline */
// Fuzz vlozeny komentar
    val current = ord(text, i)
    if current == code then /* inline */
// Fuzz vlozeny komentar

      found = found + 1
      ()

    else
      () /* inline */
      ()
    i = i + 1
  found
// Fuzz vlozeny komentar

def average(sum: Int, count: Int): Double = /* inline */
  val result = sum / (count + 0.0)
// Fuzz vlozeny komentar
  result


@main def algorithmsExample(): Unit = /* inline */

  val a = readInt() /* inline */
  val b = readInt()
  val divisor = gcd(a, b) /* inline */
  print(divisor)
  print("\n")
  val limit = readInt()
  var n = 0
  var primes = 0
  var sum = 0
// Fuzz vlozeny komentar

  while n <= limit do
    val value = fibonacci(n)
    print(value) /* inline */
// Fuzz vlozeny komentar
    print(" ")
    val prime = isPrime(n)
// Fuzz vlozeny komentar
    primes = primes + prime
    sum = sum + value

    n = n + 1
// Fuzz vlozeny komentar

  print("\n")
// Fuzz vlozeny komentar
  print(primes) /* inline */
  print("\n")
  val mean = average(sum, n)

  val meanText = toStr(mean)
  print(meanText)

  print("\n") /* inline */
  val line = repeat("ab", 3)

  print(line)
  print("\n") /* inline */

  val letters = countChar(line, 97)
  print(letters) /* inline */
  print("\n") /* inline */
