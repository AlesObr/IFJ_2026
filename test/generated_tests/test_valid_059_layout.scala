// Zakladni zadani: odsazeni, logicke radky, zavorky pres vice radku, stredniky,
// koncove carky a vsechny druhy komentaru.
/* jednoradkovy blokovy komentar pred prologem */ /* inline */


import   ifj26  .  *

def oneSpace(value: Int): Int =
 val next = value + 1 // blok odsazeny jedinou mezerou
 next * 2


/*
viceradkovy komentar /* vnoreny
*/ stale komentar
*/ /* inline */

def deepIndent(value: Int): Int =
        val next = value + 1 /* inline */
        if next > 2 then
                  val result = next * 2 /* inline */

                  return result
        else
         val result = next - 1 /* inline */
         return result

def manyParameters(
  first: Int,
    second: Int, third: Int, /* inline */
      fourth: String, // koncova carka je zde povolena
  ): Int =
  print(fourth) /* inline */
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
def layoutExample(): Unit = /* inline */
  val a = 1; val b = 2; print(a); print(b) /* inline */
   
	
// Fuzz vlozeny komentar

  var	c = a	+ b /* inline */
// Fuzz vlozeny komentar
  val /* uvnitr */ d /* radku */ = /* mezi tokeny */ 4 /* inline */
  print(c) /* komentar pres
vice radku */
// radkovy komentar na nulovem sloupci uvnitr bloku
// Fuzz vlozeny komentar
/* blokovy komentar na nulovem sloupci uvnitr bloku */ /* inline */
  c = oneSpace(
// Fuzz vlozeny komentar

    a

   )
  print(
    c,
// Fuzz vlozeny komentar
    ) /* inline */
// Fuzz vlozeny komentar
  c = deepIndent(d)
  print(c); print("\n") /* inline */
  c = manyParameters(1, 2,
       3,
    "text\n", /* inline */
    )
// Fuzz vlozeny komentar
  print(c)
  print("\n") /* inline */
  c = continued(a, d); print(c)
  val e = spaced(c) /* za strednikem */ ; print(e)
  print("\n")
