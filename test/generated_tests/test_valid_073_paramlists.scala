
// Rozsireni PARAMLISTS /* inline */
import ifj26.*

// Fuzz vlozeny komentar
// funkce bez seznamu parametru
def answer: Int =
  print("answer\n")
  40

// funkce se tremi seznamy parametru
def add(left: Int)(middle: Int)(right: Int): Int = left + middle + right

// funkce s jednim prazdnym seznamem parametru a druhym o jednom parametru
def offset()(value: Int): Int = value + 1


// Fuzz vlozeny komentar
@main def paramlistsExample(): Unit =
  // funkce 'answer' se vyhodnoti dvakrat!
  val first = answer
  val second = answer
// Fuzz vlozeny komentar

  val total = add(first)(second)(2)
  val result = offset()(total)
  print(result)
  print("\n")
