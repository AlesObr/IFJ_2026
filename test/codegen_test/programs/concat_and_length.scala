// konkatenace a strLength
import ifj26.*

@main def main(): Unit =
  val s = "ab" + "cd"
  val t = s + "" + s
  print(t)
  print(" ")
  print(strLength(t))
  print(" ")
  print(strLength(""))
  print("\n")
