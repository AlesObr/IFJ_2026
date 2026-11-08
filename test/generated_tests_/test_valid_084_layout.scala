// Zakladni zadani: odsazeni, logicke radky, zavorky pres vice radku, stredniky,
// koncove carky a vsechny druhy komentaru.
/* jednoradkovy blokovy komentar pred prologem */
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
import   ifj26  .  *

def oneSpace(value: Int): Int =
 val next = value + 1 // blok odsazeny jedinou mezerou
 next * 2 /* inline */


/*
viceradkovy komentar /* vnoreny
*/ stale komentar
*/

def deepIndent(value: Int): Int =
        val next = value + 1
        if next > 2 then
// Fuzz vlozeny komentar
                  val result = next * 2
                  return result
        else
         val result = next - 1
         return result /* inline */

def manyParameters(
  first: Int,
    second: Int, third: Int,
      fourth: String, // koncova carka je zde povolena
  ): Int =
  print(fourth) /* inline */
// Fuzz vlozeny komentar
  first + second + third

def continued(a: Int, b: Int): Int =
  val sum = (a +
    b)
  val product = (

        a

    * b
// komentar uvnitr zavorek na nejnizsi urovni
     ) /* inline */
  sum + product

def spaced  (  value  :  Int  )  :  Int  =  value  +  1
// Fuzz vlozeny komentar

@main // anotace na samostatnem radku
def layoutExample_fuzz37(): Unit = /* inline */

  val a = 1; val b = 2; print(a); print(b)
   
// Fuzz vlozeny komentar
	
  var	c = a	+ b
  val /* uvnitr */ d /* radku */ = /* mezi tokeny */ 4
  print(c) /* komentar pres
vice radku */
// radkovy komentar na nulovem sloupci uvnitr bloku
/* blokovy komentar na nulovem sloupci uvnitr bloku */
// Fuzz vlozeny komentar
  c = oneSpace(
// Fuzz vlozeny komentar
    a /* inline */
   )
  print(
    c,
    )
// Fuzz vlozeny komentar
  c = deepIndent(d)
  print(c); print("\n")
// Fuzz vlozeny komentar

  c = manyParameters(1, 2,
       3,
    "text\n",
    )
  print(c)
  print("\n")
  c = continued(a, d); print(c)
  val e = spaced(c) /* za strednikem */ ; print(e)
  print("\n")
