// Boolean promenna jako podminka if a while
import ifj26.*

@main def main(): Unit =
  var go = true
  var i = 0
  while go do
    i = i + 1
    go = i < 3
  if go then
    print("bad")
  else
    print(i)
