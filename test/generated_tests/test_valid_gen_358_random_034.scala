import ifj26.*
// Fuzz vlozeny komentar

def  /**/  f1(p2: Double): Int =
        print("a " +  "//x y//" + (
                 " text\"" + /* ) ( , = */ /* a /* nested */ b */ "0" /* inline */
           ) + " x yx yHello")
        3 / (42 /* a /* b */ c */ + 1 /* c */ * 2) * (2)

def f3(p4: Unit, /* a /* nested */ b */ p_5: Int): /* " */ Unit =
   return
          
// Fuzz vlozeny komentar

@main
def /* " */ main5(): Unit /* a /* b */ c */ /**/ =

   print("x y0Hello\"")
