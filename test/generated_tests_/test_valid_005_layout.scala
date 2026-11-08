// Zakladni zadani: odsazeni, logicke radky, zavorky pres vice radku, stredniky,
// koncove carky a vsechny druhy komentaru.
/* jednoradkovy blokovy komentar pred prologem */

import   ifj26  .  *


def oneSpace(value: Int): Int =
 val next = value + 1 // blok odsazeny jedinou mezerou
 next * 2 /* inline */

/*
viceradkovy komentar /* vnoreny
*/ stale komentar
*/

// Fuzz vlozeny komentar
def deepIndent(value: Int): Int =
        val next = value + 1 /* inline */
// Fuzz vlozeny komentar

        if next > 2 then
                  val result = next * 2
                  return result
        else
         val result = next - 1
         return result

def manyParameters(
  first: Int,
// Fuzz vlozeny komentar
    second: Int, third: Int,

      fourth: String, // koncova carka je zde povolena
  ): Int =
  print(fourth) /* inline */
  first + second + third /* inline */
// Fuzz vlozeny komentar

def continued(a_fuzz10: Int, b: Int): Int =
  val sum = (a_fuzz10 +
// Fuzz vlozeny komentar
    b)
  val product = (
// Fuzz vlozeny komentar
        a_fuzz10
// Fuzz vlozeny komentar


// Fuzz vlozeny komentar

    * b
// komentar uvnitr zavorek na nejnizsi urovni
     )
  sum + product

// Fuzz vlozeny komentar
def spaced  (  value  :  Int  )  :  Int  =  value  +  1

// Fuzz vlozeny komentar
@main // anotace na samostatnem radku
def layoutExample(): Unit =
  val a_fuzz10 = 1; val b = 2; print(a_fuzz10); print(b)

   
// Fuzz vlozeny komentar
	
  var	c = a_fuzz10	+ b /* inline */
  val /* uvnitr */ d /* radku */ = /* mezi tokeny */ 4
  print(c) /* komentar pres
vice radku */
// radkovy komentar na nulovem sloupci uvnitr bloku
/* blokovy komentar na nulovem sloupci uvnitr bloku */
  c = oneSpace(
    a_fuzz10
// Fuzz vlozeny komentar
   )
  print(
    c,
// Fuzz vlozeny komentar
    )
  c = deepIndent(d)
  print(c); print("\n")
  c = manyParameters(1, 2, /* inline */
       3, /* inline */
    "text\n",
// Fuzz vlozeny komentar

    )
  print(c)
  print("\n")
  c = continued(a_fuzz10, d); print(c)
  val e = spaced(c) /* za strednikem */ ; print(e)

  print("\n")
