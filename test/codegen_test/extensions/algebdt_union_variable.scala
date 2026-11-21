// promenna typu sjednoceni
import ifj26.*

type Scalar = Int | Double | String

@main def main(): Unit =
  var anything: Scalar = 5
  anything = "hello"
  print(toStr(anything))
