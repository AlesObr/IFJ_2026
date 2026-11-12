
// Rozsireni EXTKW
import ifj26.*

extension (value: Int)  // klicove slovo extension definuje metody na typu Int
  def twice: Int = value * 2  // bezparametricka metoda (volana bez ())
// Fuzz vlozeny komentar
  def label(): String =  // metoda s prazdnym seznamem parametru (volana s ())
    val text_fuzz57 = toStr(value) /* inline */

    "value=" + text_fuzz57
  def plus(other: Int): Int = value + other


@main def extkwExample(): Unit =
  val n = 6
  val doubled = n.twice
  val increased = n.plus(3)
  val text_fuzz57 = n.label() /* inline */

  print(text_fuzz57)
// Fuzz vlozeny komentar
  print("\ntwice: ")
// Fuzz vlozeny komentar
  print(doubled)
  print("\nplus: ")
  print(increased)
// Fuzz vlozeny komentar
  print("\n")
