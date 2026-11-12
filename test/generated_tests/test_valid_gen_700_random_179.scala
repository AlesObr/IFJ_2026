import ifj26.*
// Fuzz vlozeny komentar

def f1(): Unit /* " */ = /* inline */
  print(6e0); print(2)


@main def    main1_fuzz16(): /* * / */ Unit =
// Fuzz vlozeny komentar
        print(-59.5e-3)
        print(- (-(1 - - (100)))) /* inline */
