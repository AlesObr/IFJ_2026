import ifj26 . *
// Fuzz vlozeny komentar

def f1(p2: Double): Double /* a /* nested */ b */ = /* " */ p2

// Fuzz vlozeny komentar
@main
// Fuzz vlozeny komentar
def /* a /* nested */ b */ main2_fuzz38(): Unit = /* inline */
        print(5e2d); print(7)
        val /* c */ v3X = f1(5E+0) + 6e2d + f1(.80)
