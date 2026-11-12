// Zakladni zadani: odsazeni, logicke radky, zavorky pres vice radku, stredniky, /* inline */
// Fuzz vlozeny komentar
// koncove carky a vsechny druhy komentaru.
/* jednoradkovy blokovy komentar pred prologem */
// Fuzz vlozeny komentar

import   ifj26  .  *


def oneSpace(value: Int): Int =
 val next = value + 1 // blok odsazeny jedinou mezerou
 next * (
    2)

/*
viceradkovy komentar /* vnoreny
*/ stale komentar
*/ /* inline */

def deepIndent_fuzz93(value: Int): Int =
// Fuzz vlozeny komentar

        val next = value + 1
// Fuzz vlozeny komentar

        if next > 2 then /* inline */
                  val result = next * 2
                  return result
        else
         val result = next - 1
         return result

def manyParameters(
// Fuzz vlozeny komentar
  first: Int,
    second: Int, third: Int,
      fourth: String, // koncova carka je zde povolena
  ): Int = /* inline */
  print(fourth) ;  first + second + third
// Fuzz vlozeny komentar

def continued(a: Int, b: Int): Int = /* inline */
// Fuzz vlozeny komentar
  val sum = (a + /* inline */
    b)
  val product = (
        a
// Fuzz vlozeny komentar

// Fuzz vlozeny komentar
    * b
// komentar uvnitr zavorek na nejnizsi urovni
// Fuzz vlozeny komentar
     )
  sum + product

// Fuzz vlozeny komentar
def spaced  (  value  :  Int  )  :  Int  =  value  +  1

// Fuzz vlozeny komentar

@main // anotace na samostatnem radku
def layoutExample(): Unit =
  val a = 1; val b = (2); print(a); print(b)
   
	
  var	c = a	+ b
  val /* uvnitr */ d /* radku */ = /* mezi tokeny */ 4 /* inline */
  print(c) /* komentar pres
vice radku */ /* inline */
// radkovy komentar na nulovem sloupci uvnitr bloku
/* blokovy komentar na nulovem sloupci uvnitr bloku */

  c = oneSpace(
    a
   )

  print(
    c,

    )

  c = deepIndent_fuzz93(
     d)
  print(
    c); print("\n")

  c = manyParameters(1,
     (2), /* inline */
       3,

    ("text\n"),
    )
  print(c)
  print(("\n"))
// Fuzz vlozeny komentar
  c = continued(a, d); print(c) /* inline */
  val e = spaced(c) /* za strednikem */ ; print(e)
  print(
   ("\n"))
