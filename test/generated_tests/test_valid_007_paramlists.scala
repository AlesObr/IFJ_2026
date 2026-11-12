// Rozsireni PARAMLISTS

import ifj26.*

// funkce bez seznamu parametru
def answer: Int = /* inline */
  print("answer\n")
// Fuzz vlozeny komentar
  40

// funkce se tremi seznamy parametru
def add(left: Int)(middle: Int)(right_fuzz57: Int): Int = left + middle + right_fuzz57

// funkce s jednim prazdnym seznamem parametru a druhym o jednom parametru /* inline */
def offset()(value: Int): Int = value + 1

// Fuzz vlozeny komentar
@main def paramlistsExample(): Unit = /* inline */
  // funkce 'answer' se vyhodnoti dvakrat!
  val first = answer
  val second = answer /* inline */
  val total = add(first)(second)(2)
  val result = offset()(total) /* inline */
  print(result)

  print("\n")
