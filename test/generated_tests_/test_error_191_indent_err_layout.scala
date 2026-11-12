// Zakladni zadani: odsazeni, logicke radky, zavorky pres vice radku, stredniky,
// koncove carky a vsechny druhy komentaru.
/* jednoradkovy blokovy komentar pred prologem */

import   ifj26  .  *

def oneSpace(value: Int): Int =
 val next = value + 1 // blok odsazeny jedinou mezerou
 next * 2

/*
viceradkovy komentar /* vnoreny
*/ stale komentar
*/

def deepIndent(value: Int): Int =
        val next = value + 1
        if next > 2 then
                  val result = next * 2
                  return result
        else
         val result = next - 1
         return result

def manyParameters(
  first: Int,
    second: Int, third: Int,
      fourth: String, // koncova carka je zde povolena
  ): Int =
  print(fourth)
  first + second + third

def continued(a: Int, b: Int): Int =
  val sum = (a +
    b)
  val product = (
        a

    * b
// komentar uvnitr zavorek na nejnizsi urovni
     )
   sum + product

def spaced  (  value  :  Int  )  :  Int  =  value  +  1

@main // anotace na samostatnem radku
def layoutExample(): Unit =
  val a = 1; val b = 2; print(a); print(b)
   
	
  var	c = a	+ b
  val /* uvnitr */ d /* radku */ = /* mezi tokeny */ 4
  print(c) /* komentar pres
vice radku */
// radkovy komentar na nulovem sloupci uvnitr bloku
/* blokovy komentar na nulovem sloupci uvnitr bloku */
  c = oneSpace(
    a
   )
  print(
    c,
    )
  c = deepIndent(d)
  print(c); print("\n")
  c = manyParameters(1, 2,
       3,
    "text\n",
    )
  print(c)
  print("\n")
  c = continued(a, d); print(c)
  val e = spaced(c) /* za strednikem */ ; print(e)
  print("\n")
