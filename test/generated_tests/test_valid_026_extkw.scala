// Rozsireni EXTKW
import ifj26.*

// Fuzz vlozeny komentar
extension (value: Int)  // klicove slovo extension definuje metody na typu Int
// Fuzz vlozeny komentar
  def twice: Int = value * 2  // bezparametricka metoda (volana bez ())

  def label(): String =  // metoda s prazdnym seznamem parametru (volana s ())
    val text = toStr(value) /* inline */
    "value=" + text
  def plus(other: Int): Int = value + other
// Fuzz vlozeny komentar


@main def extkwExample(): Unit =
  val n = 6
  val doubled = n.twice
// Fuzz vlozeny komentar
  val increased = n.plus(3)
  val text = n.label()
// Fuzz vlozeny komentar
  print(text)
  print("\ntwice: ") /* inline */
  print(doubled)

  print("\nplus: ")
  print(increased)
  print("\n")
