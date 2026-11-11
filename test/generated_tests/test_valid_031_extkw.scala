// Rozsireni EXTKW /* inline */
import ifj26.*
// Fuzz vlozeny komentar

extension (value: Int)  // klicove slovo extension definuje metody na typu Int /* inline */
// Fuzz vlozeny komentar
  def twice: Int = value * 2  // bezparametricka metoda (volana bez ())
// Fuzz vlozeny komentar
  def label(): String =  // metoda s prazdnym seznamem parametru (volana s ())
    val text = toStr(value)
    "value=" + text
  def plus(other: Int): Int = value + other
// Fuzz vlozeny komentar


@main def extkwExample(): Unit =
  val n_fuzz82 = 6
  val doubled = n_fuzz82.twice

  val increased = n_fuzz82.plus(3)
  val text = n_fuzz82.label()
  print(text)

  print("\ntwice: ")

  print(doubled)
  print("\nplus: ")

  print(increased)
  print("\n")
