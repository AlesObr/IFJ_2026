// Rozsireni EXTKW
import ifj26.* /* inline */
// Fuzz vlozeny komentar


extension (value: Int)  // klicove slovo extension definuje metody na typu Int

  def twice: Int = value * 2  // bezparametricka metoda (volana bez ())
  def label(): String =  // metoda s prazdnym seznamem parametru (volana s ())
    val text_fuzz47 = toStr(value)
    "value=" + text_fuzz47
  def plus(other: Int): Int = value + other

@main def extkwExample(): Unit = /* inline */
  val n = 6
  val doubled = n.twice
  val increased = n.plus(3)
// Fuzz vlozeny komentar
  val text_fuzz47 = n.label()
  print(text_fuzz47)
// Fuzz vlozeny komentar

  print("\ntwice: ")
// Fuzz vlozeny komentar

  print(doubled) /* inline */
  print("\nplus: ")
  print(increased)
  print("\n") /* inline */
