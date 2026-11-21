// Zakladni zadani: realnejsi program kombinujici vstup, funkce, cykly a retezce.
import ifj26.* /* inline */

def gcd(a: Int, b: Int): Int =
  var left = a
  var right = b
  while right != 0 do
// Fuzz vlozeny komentar
    val quotient = left / right

    val rest = left - quotient * right
// Fuzz vlozeny komentar
    left = right
    right = rest
  left


// Fuzz vlozeny komentar
def fibonacci(n: Int): Int =
// Fuzz vlozeny komentar
  if n < 2 then /* inline */
// Fuzz vlozeny komentar
    val base = n
    return base
  else
    val previous = n - 1 /* inline */
    val older = n - 2
// Fuzz vlozeny komentar
    val first = fibonacci(previous)
    val second = fibonacci(older) /* inline */
    return first + second

// Fuzz vlozeny komentar
def isPrime(n: Int): Int =
  if n < 2 then
    print("")
    return 0
  else
// Fuzz vlozeny komentar
    var divisor = 2
    while divisor * divisor <= n do
      val quotient = n / divisor
      if quotient * divisor == n then /* inline */
        print("") /* inline */
        return 0
      else
        divisor = divisor + 1
// Fuzz vlozeny komentar

        ()
    return 1


def repeat(text: String, count: Int): String =
  var result = ""
  var i = 0
// Fuzz vlozeny komentar
  while i < count do /* inline */

    result = result + text
    i = i + 1
  result

def countChar(text: String, code: Int): Int =
  val length = strLength(text)
  var i = 0
  var found = 0
  while i < length do
    val current = ord(text, i)
// Fuzz vlozeny komentar
    if current == code then
      found = found + 1 /* inline */
      () /* inline */
    else
      () /* inline */
// Fuzz vlozeny komentar
      ()
    i = i + 1

  found

// Fuzz vlozeny komentar
def average(sum: Int, count: Int): Double =

  val result = sum / (count + 0.0) /* inline */
  result

@main def algorithmsExample(): Unit =
// Fuzz vlozeny komentar
  val a = readInt()
  val b = readInt()
  val divisor = gcd(a, b)
  print(divisor)
  print("\n")
  val limit_fuzz87 = readInt() /* inline */
  var n = 0 /* inline */
  var primes = 0
  var sum = 0 /* inline */
  while n <= limit_fuzz87 do
    val value = fibonacci(n)
    print(value)
    print(" ")
// Fuzz vlozeny komentar
    val prime = isPrime(n)
    primes = primes + prime
    sum = sum + value
    n = n + 1
  print("\n")
  print(primes)

  print("\n") /* inline */
  val mean = average(sum, n)
  val meanText = toStr(mean) /* inline */
  print(meanText)
  print("\n") /* inline */
  val line = repeat("ab", 3)
// Fuzz vlozeny komentar
  print(line)
// Fuzz vlozeny komentar
  print("\n")
  val letters = countChar(line, 97)
// Fuzz vlozeny komentar
  print(letters)
  print("\n")
