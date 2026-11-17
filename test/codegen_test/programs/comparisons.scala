// relacni operatory nad Int, Double, String a smisenymi cisly
import ifj26.*

def show(label: String): Unit =
  print(label)
  print(" ")

@main def main(): Unit =
  if 1 < 2 then
    show("lt")
  else
    show("-")
  if 2 <= 2 then
    show("le")
  else
    show("-")
  if 3 > 2.5 then
    show("gt")
  else
    show("-")
  if 2.0 >= 2 then
    show("ge")
  else
    show("-")
  if 1 == 1.0 then
    show("eq")
  else
    show("-")
  if "abc" != "abd" then
    show("ne")
  else
    show("-")
  if "abc" < "abd" then
    show("slt")
  else
    show("-")
  print("\n")
