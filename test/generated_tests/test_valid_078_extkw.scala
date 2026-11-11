
// Rozsireni EXTKW
import ifj26.*

extension (value: Int)  // klicove slovo extension definuje metody na typu Int

  def twice: Int = value * 2  // bezparametricka metoda (volana bez ()) /* inline */

  def label(): String =  // metoda s prazdnym seznamem parametru (volana s ())
    val text = toStr(value)
// Fuzz vlozeny komentar

    "value=" + text /* inline */
  def plus(other: Int): Int = value + other /* inline */


@main def extkwExample(): Unit =
  val n_fuzz83 = 6
  val doubled = n_fuzz83.twice
  val increased = n_fuzz83.plus(3) /* inline */
  val text = n_fuzz83.label()

  print(text)
  print("\ntwice: ")

  print(doubled)
  print("\nplus: ")
  print(increased)
  print("\n")
