// prekryti promenne v podbloku, puvodni hodnota zustane
import ifj26.*

@main def main(): Unit =
  val x = 1
  if x < 2 then
    val x = "inner"
    print(x)
  else
    ()
  print(x)
  var y = 10
  while y > 8 do
    val y2 = y
    print(y2)
    y = y - 1
  print("\n")
