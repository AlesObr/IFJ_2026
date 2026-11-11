// Zakladni zadani: odsazeni, logicke radky, zavorky pres vice radku, stredniky,
// Fuzz vlozeny komentar
// koncove carky a vsechny druhy komentaru.
/* jednoradkovy blokovy komentar pred prologem */

import   ifj26  .  *


def oneSpace(value: Int): Int =
 val next = value + 1 // blok odsazeny jedinou mezerou
 next * 2
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
/*
viceradkovy komentar /* vnoreny
*/ stale komentar
*/

// Fuzz vlozeny komentar

def deepIndent(value: Int): Int =
        val next = value + 1
        if next > 2 then
                  val result = next * 2
// Fuzz vlozeny komentar
                  return result
        else
         val result = next - 1
         return result

// Fuzz vlozeny komentar
def manyParameters(
// Fuzz vlozeny komentar
  first: Int,
    second: Int, third: Int,
      fourth: String, // koncova carka je zde povolena
  ): Int =
  print(fourth)
  first + second + third

// Fuzz vlozeny komentar
def continued(a: Int, b: Int): Int =
  val sum = (a +
// Fuzz vlozeny komentar
    b)
  val product = (

        a

    * b

// komentar uvnitr zavorek na nejnizsi urovni
// Fuzz vlozeny komentar
     )

  sum + product

def spaced  (  value  :  Int  )  :  Int  =  value  +  1 /* inline */


// Fuzz vlozeny komentar
@main // anotace na samostatnem radku /* inline */
def layoutExample(): Unit = /* inline */
  val a = 1; val b = 2; print(a); print(b)
// Fuzz vlozeny komentar
   
	
  var	c = a	+ b /* inline */
// Fuzz vlozeny komentar
  val /* uvnitr */ d_fuzz26 /* radku */ = /* mezi tokeny */ 4
  print(c) /* komentar pres
vice radku */
// radkovy komentar na nulovem sloupci uvnitr bloku
/* blokovy komentar na nulovem sloupci uvnitr bloku */
  c = oneSpace( /* inline */
    a
   ) /* inline */
  print(
    c,
// Fuzz vlozeny komentar
    )
  c = deepIndent(d_fuzz26)
  print(c); print("\n")
  c = manyParameters(1, 2,
       3,
    "text\n",
    )
  print(c)
  print("\n")
  c = continued(a, d_fuzz26); print(c)
  val e = spaced(c) /* za strednikem */ ; print(e)
// Fuzz vlozeny komentar
  print("\n")
