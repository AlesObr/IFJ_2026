// dlouhy retezec skladany v cyklu
import ifj26.*

@main def main(): Unit =
  var s = ""
  var i = 0
  while i < 50 do
    s = s + chr(97 + i - i / 26 * 26)
    i = i + 1
  print(s)
  print(" ")
  print(strLength(s))
  print("\n")
