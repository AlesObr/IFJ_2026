// bezparametricka rozsirujici metoda
import ifj26.*

extension (value: Int)
  def twice: Int = value * 2

@main def main(): Unit =
  val x: Int = 6
  print(x.twice)
