// Rozsireni EXTKW
import ifj26.*

extension (value: Int)  // klicove slovo extension definuje metody na typu Int
  def twice: Int = value * 2  // bezparametricka metoda (volana bez ()) /* inline */
// Fuzz vlozeny komentar
  def label(): String =  // metoda s prazdnym seznamem parametru (volana s ())
// Fuzz vlozeny komentar

    val text = toStr(value)
    "value=" + text /* inline */
  def plus(other: Int): Int = value + other

@main def extkwExample(): Unit =
// Fuzz vlozeny komentar
  val n = 6 /* inline */
  val doubled = n.twice /* inline */
  val increased = n.plus(3) /* inline */
  val text = n.label()
// Fuzz vlozeny komentar
  print(text)

  print("\ntwice: ") /* inline */
  print(doubled)
// Fuzz vlozeny komentar
  print("\nplus: ")
  print(increased)
  print("\n")
