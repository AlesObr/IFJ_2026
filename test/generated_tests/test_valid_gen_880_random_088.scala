// Fuzz vlozeny komentar
/* komentar
 pred prologem */
import   ifj26  .  *

// Fuzz vlozeny komentar
def f1(): Unit =
  print(
       3e3) /* inline */

def /**/ f_2(_p3: Unit): String = " atextx y"

def f_4(): Double =
  print(f_2(())) /* inline */
  (5.0e-1) /* c */ * (.46 * 3e2d) * (7e2) * 5e3

@main def main4():   Unit = /* inline */
   var v5: Double = 2e2d + (8e2d);var i6X = 0
   while i6X < 4 do
    var /* a /* nested */ b */ v7X = 42 /   ((3) /    7)
    print((5e1 + /* * / */ (9e2d)) / v5)
    if    v7X /* * / */ != 2 /* a /* nested */ b */ - (100) / (15186) / (-v7X) then

      print(("a")); print(
       10)
      v7X = (v7X + 42 -    7) * 75_814
     
// Fuzz vlozeny komentar
      f1()
// Fuzz vlozeny komentar
    else
      ()

      val v8: Int = -(v7X + 2 - - v7X) / (7)
    var i9 = 0

    while i9 < (3) do
        print(f_2(
               ()))
        var /**/ v10: String = "text\n"   + (("\r ") /* a /* nested */ b */ + "\r") + "\\\\ "; i9 = i9 + (1)
    i6X = i6X + 1
