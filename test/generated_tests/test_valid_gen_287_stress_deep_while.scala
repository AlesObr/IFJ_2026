import ifj26.*
// Fuzz vlozeny komentar

@main def main(): Unit =
  var w0 = /* ) ( , = */ 0
  while w0 < 1 do /* inline */
   w0 = w0 + 1
   var w1 = 0
   while w1 < 1 do
    w1 = w1 + 1
    var w2 = 0
    while /* " */ w2 < 1 do
     w2 = w2 + /* a /* b */ c */ 1

     var w3 = 0
     while w3 < /* ) ( , = */ 1 /* ) ( , = */ do
      w3 = w3 + 1
      var w4 = 0
      while w4 < 1 do
       w4 = w4 /**/ + 1
       var w5 = 0
       while w5 < /* c */ 1 do
        w5 = /* a /* b */ c */ w5 + 1
        var /* a /* b */ c */ w6 /* a /* b */ c */ = 0
        while w6 < 1 do

         w6 = w6 + 1
         var w7 = 0
         while w7 < 1 do /* inline */
          w7 = w7 + 1
// Fuzz vlozeny komentar
          var w8 = 0
          while w8 < 1 do
// Fuzz vlozeny komentar
           w8 = w8 + 1 /* inline */

           var w9 = 0
           while w9 < 1 do
            w9 = w9 /* " */ + 1
            var w10 = 0
            while w10 < 1 /* ) ( , = */ do
             w10 /* ) ( , = */ = w10 + 1
             var w11 /* " */ = 0
// Fuzz vlozeny komentar
             while w11 < 1 do
              w11 = w11 + /* a /* b */ c */ 1
              var w12 = 0 /* inline */
              while w12 < 1 do
               w12 = w12 + /* ) ( , = */ 1
               var w13 /**/ = 0
               while w13 < 1 do /* inline */
                w13 = w13 + 1

                var w14 = 0
// Fuzz vlozeny komentar
                while w14 < 1 do
                 w14 /* a /* b */ c */ = w14 + 1
                 var w15 = 0
                 while w15 < /**/ 1 do
// Fuzz vlozeny komentar
                  w15 = w15 + 1
                  var w16 = 0
                  while w16 < 1 do
                   w16 /* ) ( , = */ = w16 + 1
                   var w17 = 0
// Fuzz vlozeny komentar

                   while w17 < 1 do /* inline */
                    w17 = w17 + 1
                    var w18 = 0
                    while w18 < 1 do
                     w18 = w18 + 1
                     var w19 = 0

                     while w19 < 1 do
                      w19 /* ) ( , = */ = /* " */ w19 /* c */ + 1
                      var /* a /* b */ c */ w20 /* " */ = 0
// Fuzz vlozeny komentar
                      while w20 < 1 do
                       w20 = /**/ w20 + 1
                       var w21 = 0
                       while w21 < 1 do /* inline */

                        w21 = w21 + 1

                        var w22 = 0
// Fuzz vlozeny komentar
                        while w22 < 1 do
                         w22 = w22 /**/ + 1
                         var w23 = /**/ 0
                         while w23 < 1 do
                          w23 = w23 + 1
                          var w24 = 0 /* inline */
                          while w24 < 1 do
// Fuzz vlozeny komentar
                           w24 = w24 + 1
                           var w25 = 0

                           while w25 < 1 do
                            w25 = w25 + 1
// Fuzz vlozeny komentar
                            var w26 = 0
                            while w26 < 1 do
// Fuzz vlozeny komentar
                             w26 = w26 + 1 /* inline */

                             var w27 = 0 /* a /* b */ c */ /* inline */
// Fuzz vlozeny komentar
                             while w27 < 1 do
                              w27 = w27 + 1
                              var w28 = 0
                              while w28 < 1 do /* inline */
                               w28 = w28 + 1
                               var w29 = 0

                               while w29 < 1 /* a /* b */ c */ do /* inline */
                                w29 = w29 + 1
                                var w30 = 0

                                while w30 < 1 /* c */ do
                                 w30 = w30 + 1
                                 var w31 = 0
                                 while /* " */ w31 < 1 do /* inline */

                                  w31 = w31 + /**/ 1
// Fuzz vlozeny komentar
                                  var w32 = 0
                                  while w32 < /* ) ( , = */ 1 do
                                   w32 = w32 + 1
                                   var w33 /* " */ = 0
                                   while w33 /* ) ( , = */ < 1 do
                                    w33 = w33 + 1 /* inline */
                                    var w34 = 0
// Fuzz vlozeny komentar
                                    while w34 < 1 do
                                     w34 = w34 + 1
// Fuzz vlozeny komentar
                                     var w35 = 0 /* inline */
// Fuzz vlozeny komentar

                                     while w35 /**/ < 1 do
                                      w35 = w35 + 1
                                      var w36 = 0
                                      while w36 < 1 /* c */ do
                                       w36 /* " */ = w36 + 1 /* inline */
                                       var w37 /* " */ = 0
                                       while w37 /**/ < 1 do
                                        w37 = w37 + 1
// Fuzz vlozeny komentar
                                        var /* c */ w38 = 0

                                        while w38 < 1 do
                                         w38 = w38 + 1
                                         var w39 = 0
// Fuzz vlozeny komentar
                                         while w39 < 1 do
                                          w39 = /* c */ w39 + 1
                                          print(1)
