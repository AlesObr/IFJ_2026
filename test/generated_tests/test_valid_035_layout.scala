// Fuzz vlozeny komentar
// Zakladni zadani: odsazeni, logicke radky, zavorky pres vice radku, stredniky,
// koncove carky a vsechny druhy komentaru.
/* jednoradkovy blokovy komentar pred prologem */
// Fuzz vlozeny komentar

import   ifj26  .  *

// Fuzz vlozeny komentar
def oneSpace(value: Int): Int =
 val next = value + 1 // blok odsazeny jedinou mezerou /* inline */
 next * 2

/*
viceradkovy komentar /* vnoreny
*/ stale komentar
*/



def deepIndent(value: Int): Int =
        val next = value + 1
        if next > 2 then
                  val result = next * 2
                  return result /* inline */
        else
         val result = next - 1

         return result

def manyParameters(
  first: Int,
    second: Int, third: Int,
      fourth: String, // koncova carka je zde povolena
  ): Int =
  print(fourth)

  first + second + third /* inline */

def continued(a: Int, b: Int): Int =
  val sum = (a +
// Fuzz vlozeny komentar
    b)
  val product = (
        a


// Fuzz vlozeny komentar
    * b

// komentar uvnitr zavorek na nejnizsi urovni /* inline */
     )
  sum + product

def spaced  (  value  :  Int  )  :  Int  =  value  +  1
// Fuzz vlozeny komentar

@main // anotace na samostatnem radku /* inline */
def layoutExample(): Unit =
  val a = 1; val b = 2; print(a); print(b)
   
	
  var	c = a	+ b
  val /* uvnitr */ d /* radku */ = /* mezi tokeny */ 4
  print(c) /* komentar pres
vice radku */
// Fuzz vlozeny komentar
// radkovy komentar na nulovem sloupci uvnitr bloku
/* blokovy komentar na nulovem sloupci uvnitr bloku */ /* inline */
  c = oneSpace(
    a
   )
// Fuzz vlozeny komentar
  print(
    c, /* inline */
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
// Fuzz vlozeny komentar
  val e = spaced(c) /* za strednikem */ ; print(e)
  print("\n")
