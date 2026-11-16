// return z cyklu a samostatny return
import ifj26.*

def find(limit: Int): Int =
  var i = 0
  while i < limit do
    if i * i > 20 then
      return i
    else
      i = i + 1
  0 - 1

def early(x: Int): Unit =
  if x < 0 then
    print("neg")
    return
  else
    print("pos")
  print("!")

@main def main(): Unit =
  print(find(100))
  print(" ")
  print(find(3))
  print(" ")
  early(-1)
  early(1)
  print("\n")
