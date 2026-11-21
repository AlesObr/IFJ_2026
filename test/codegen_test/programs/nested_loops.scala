// vnorene cykly a definice v tele (kazda iterace)
import ifj26.*

@main def main(): Unit =
  var row = 1
  while row <= 3 do
    var col = 0
    while col < row do
      val mark = "*"
      print(mark)
      col = col + 1
    print("\n")
    row = row + 1
