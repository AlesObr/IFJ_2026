// Rozsireni EXTKW
import ifj26.*

extension (value: Int)  // klicove slovo extension definuje metody na typu Int
  def twice_fuzz89: Int = value * 2  // bezparametricka metoda (volana bez ())
  def label(): String =  // metoda s prazdnym seznamem parametru (volana s ())
    val text = toStr(value)

    "value=" + text

  def plus(other: Int): Int = value + other

@main def extkwExample(): Unit =
// Fuzz vlozeny komentar

  val n = 6 /* inline */
  val doubled = n.twice
  val increased = n.plus(3)
  val text = n.label()
  print(text)
  print("\ntwice: ")
  print(doubled)
  print("\nplus: ")
// Fuzz vlozeny komentar
  print(increased)
  print("\n") /* inline */
