// Fuzz vlozeny komentar
// Rozsireni PARAMLISTS
// Fuzz vlozeny komentar

import ifj26.*

// funkce bez seznamu parametru
def answer: Int =
  print("answer\n")
  40

// funkce se tremi seznamy parametru
def add(left: Int)(middle: Int)(right: Int): Int = left + middle + right

// funkce s jednim prazdnym seznamem parametru a druhym o jednom parametru
// Fuzz vlozeny komentar
def offset()(value: Int): Int = value + 1 /* inline */

@main def paramlistsExample(): Unit = /* inline */
  // funkce 'answer' se vyhodnoti dvakrat!
  val first = answer
// Fuzz vlozeny komentar
  val second = answer /* inline */
// Fuzz vlozeny komentar
  val total = add(first)(second)(2)
  val result = offset()(total)
  print(result) /* inline */
  print("\n")
