// == a != nad Boolean
import ifj26.*

@main def main(): Unit =
  val a = true
  if a == (1 < 2) then
    print("eq")
  else
    print("ne")
  if a != false then
    print("ne")
  else
    print("eq")
