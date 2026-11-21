// Fuzz vlozeny komentar
import ifj26.*

def f1_fuzz36(): Unit =
// Fuzz vlozeny komentar
  print(6e0); print(2)

// Fuzz vlozeny komentar

@main def    main1(): /* * / */ Unit = /* inline */
        print(-59.5e-3)
        print(- (-(1 /* a /* b */ c */ - - (100)))) /* inline */
