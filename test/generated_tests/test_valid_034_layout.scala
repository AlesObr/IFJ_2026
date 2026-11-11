// Zakladni zadani: odsazeni, logicke radky, zavorky pres vice radku, stredniky,
// koncove carky a vsechny druhy komentaru. /* inline */
/* jednoradkovy blokovy komentar pred prologem */

import   ifj26  .  *

def oneSpace(value: Int): Int =
 val next = value + 1 // blok odsazeny jedinou mezerou
// Fuzz vlozeny komentar
 next * 2

/*
viceradkovy komentar /* vnoreny
*/ stale komentar
*/

def deepIndent(value: Int): Int =
        val next = value + 1
// Fuzz vlozeny komentar
        if next > 2 then
                  val result = next * 2 /* inline */
                  return result
        else
         val result = next - 1
         return result /* inline */
// Fuzz vlozeny komentar


// Fuzz vlozeny komentar
def manyParameters( /* inline */
  first: Int,
    second: Int, third: Int,
// Fuzz vlozeny komentar
      fourth: String, // koncova carka je zde povolena /* inline */
// Fuzz vlozeny komentar
  ): Int =
  print(fourth)
// Fuzz vlozeny komentar
  first + second + third

def continued(a: Int, b: Int): Int =
  val sum = (a +
    b)
  val product = ( /* inline */
        a

    * b
// komentar uvnitr zavorek na nejnizsi urovni
     )

  sum + product
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
def spaced  (  value  :  Int  )  :  Int  =  value  +  1

@main // anotace na samostatnem radku /* inline */
def layoutExample(): Unit = /* inline */
// Fuzz vlozeny komentar
  val a = 1; val b = 2; print(a); print(b)
   
	
// Fuzz vlozeny komentar
  var	c = a	+ b
  val /* uvnitr */ d_fuzz69 /* radku */ = /* mezi tokeny */ 4
  print(c) /* komentar pres
vice radku */
// radkovy komentar na nulovem sloupci uvnitr bloku
// Fuzz vlozeny komentar
/* blokovy komentar na nulovem sloupci uvnitr bloku */
  c = oneSpace(
    a
// Fuzz vlozeny komentar
   )
  print(
    c,
// Fuzz vlozeny komentar
    )
// Fuzz vlozeny komentar
  c = deepIndent(d_fuzz69)
  print(c); print("\n")
  c = manyParameters(1, 2,
       3,
    "text\n",
    )
  print(c)

  print("\n")
  c = continued(a, d_fuzz69); print(c)
  val e = spaced(c) /* za strednikem */ ; print(e)
  print("\n")
