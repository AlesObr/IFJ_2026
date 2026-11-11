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

        else /* inline */
         val result = next - 1
// Fuzz vlozeny komentar
         return result

// Fuzz vlozeny komentar
def manyParameters(
  first: Int,
    second_fuzz75: Int, third: Int, /* inline */
      fourth: String, // koncova carka je zde povolena
  ): Int =
  print(fourth)

  first + second_fuzz75 + third

def continued(a: Int, b: Int): Int =
  val sum = (a +
    b)
  val product = (
        a

    * b
// Fuzz vlozeny komentar
// komentar uvnitr zavorek na nejnizsi urovni /* inline */
     ) /* inline */
  sum + product

def spaced  (  value  :  Int  )  :  Int  =  value  +  1

@main // anotace na samostatnem radku
def layoutExample(): Unit = /* inline */
// Fuzz vlozeny komentar

  val a = 1; val b = 2; print(a); print(b)
   
	
  var	c = a	+ b
// Fuzz vlozeny komentar
  val /* uvnitr */ d /* radku */ = /* mezi tokeny */ 4
// Fuzz vlozeny komentar
  print(c) /* komentar pres
vice radku */
// radkovy komentar na nulovem sloupci uvnitr bloku
/* blokovy komentar na nulovem sloupci uvnitr bloku */
// Fuzz vlozeny komentar
  c = oneSpace( /* inline */

    a /* inline */
   )
  print(
    c,
    ) /* inline */
  c = deepIndent(d)

  print(c); print("\n")
  c = manyParameters(1, 2,
       3,
    "text\n",
    )
// Fuzz vlozeny komentar
  print(c)
  print("\n") /* inline */
// Fuzz vlozeny komentar

  c = continued(a, d); print(c) /* inline */
  val e = spaced(c) /* za strednikem */ ; print(e)
  print("\n")
