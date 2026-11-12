// Rozsireni EXTKW
import ifj26.*
// Fuzz vlozeny komentar

extension (value: Int)  // klicove slovo extension definuje metody na typu Int
  def twice: Int = value * 2  // bezparametricka metoda (volana bez ())
  def label(): String =  // metoda s prazdnym seznamem parametru (volana s ())
    val text = toStr(value)
    "value=" + text /* inline */
  def plus(other: Int): Int = value + other

@main def extkwExample(): Unit =

  val n = 6

  val doubled = n.twice
  val increased = n.plus(3)
// Fuzz vlozeny komentar
  val text = n.label()
  print(text)
  print("\ntwice: ")

  print(doubled) /* inline */
  print("\nplus: ") /* inline */
  print(increased) /* inline */
  print("\n")
