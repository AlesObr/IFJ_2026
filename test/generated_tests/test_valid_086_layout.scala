// Zakladni zadani: odsazeni, logicke radky, zavorky pres vice radku, stredniky,
// koncove carky a vsechny druhy komentaru.
/* jednoradkovy blokovy komentar pred prologem */


import   ifj26  .  *
// Fuzz vlozeny komentar


// Fuzz vlozeny komentar

def oneSpace(value: Int): Int =
 val next = value + 1 // blok odsazeny jedinou mezerou
 next * 2

/*
viceradkovy komentar /* vnoreny
*/ stale komentar
*/

def deepIndent(value: Int): Int =
// Fuzz vlozeny komentar

        val next = value + 1
        if next > 2 then
                  val result = next * 2 /* inline */
                  return result
        else
         val result = next - 1
         return result

def manyParameters(
  first: Int,
    second_fuzz26: Int, third: Int,
      fourth: String, // koncova carka je zde povolena
  ): Int = /* inline */
  print(fourth)
  first + second_fuzz26 + third

def continued(a: Int, b: Int): Int = /* inline */
  val sum = (a +
    b) /* inline */
  val product = (
        a

    * b
// komentar uvnitr zavorek na nejnizsi urovni
     )
  sum + product

// Fuzz vlozeny komentar
def spaced  (  value  :  Int  )  :  Int  =  value  +  1

// Fuzz vlozeny komentar
@main // anotace na samostatnem radku
def layoutExample(): Unit =
  val a = 1; val b = 2; print(a); print(b)
// Fuzz vlozeny komentar
   
	
  var	c = a	+ b
  val /* uvnitr */ d /* radku */ = /* mezi tokeny */ 4
  print(c) /* komentar pres
vice radku */
// radkovy komentar na nulovem sloupci uvnitr bloku
/* blokovy komentar na nulovem sloupci uvnitr bloku */
  c = oneSpace(
    a
   )
// Fuzz vlozeny komentar
  print(

    c, /* inline */
    )

  c = deepIndent(d)
// Fuzz vlozeny komentar
  print(c); print("\n")
  c = manyParameters(1, 2,
       3,
    "text\n", /* inline */
    )
  print(c)
  print("\n")
  c = continued(a, d); print(c)
// Fuzz vlozeny komentar
  val e = spaced(c) /* za strednikem */ ; print(e) /* inline */
  print("\n")
