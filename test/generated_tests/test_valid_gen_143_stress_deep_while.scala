import ifj26.*

@main def main(): Unit =
  var w0 = (0)

  while w0 < 1 do
   w0 = w0 + 1
// Fuzz vlozeny komentar
   var w1 = 0

   while w1 < 1 do
    w1 = w1 + 1 /* inline */
    var w2 = 0
    while w2 < 1 do
     w2 = w2 + (1);var w3 = (0)
     while w3 < 1 do
// Fuzz vlozeny komentar
      w3 = w3 + 1
      var w4 = 0
      while w4 < 1 do
       w4 = w4 + (1) /* inline */
       var w5 = 0
       while w5 < 1 do
// Fuzz vlozeny komentar
        w5 = w5 + (1)
        var w6 = 0
        while w6 < (1) do /* inline */

         w6 = w6 + 1
         var w7 = 0
         while w7 < 1 do
          w7 = w7 + 1
          var w8 = (0) /* inline */
          while w8 < 1 do
           w8 = w8 + 1 ;  var w9 = 0
           while w9 < (1) do
// Fuzz vlozeny komentar
            w9 = w9 + 1
            var w10 = 0
            while w10 < (1) do

             w10 = w10 + 1 /* inline */
             var w11 = 0
             while w11 < 1 do
// Fuzz vlozeny komentar

              w11 = w11 + 1
              var w12 = 0
// Fuzz vlozeny komentar
              while w12 < (1) do /* inline */
               w12 = w12 + 1 ;  var w13 = 0
               while w13 < 1 do
                w13 = w13 + 1 ;  var w14 = 0
                while w14 < 1 do
// Fuzz vlozeny komentar
                 w14 = w14 + 1
                 var w15 = 0
                 while w15 < 1 do
                  w15 = w15 + 1
                  var w16 = 0
                  while w16 < 1 do
                   w16 = w16 + 1
                   var w17 = 0
                   while w17 < (1) do

                    w17 = w17 + (1)
                    var w18 = 0
                    while w18 < 1 do
                     w18 = w18 + 1
                     var w19_fuzz60 = 0
                     while w19_fuzz60 < 1 do
                      w19_fuzz60 = w19_fuzz60 + 1
                      var w20 = 0
                      while w20 < 1 do
                       w20 = w20 + (1)
// Fuzz vlozeny komentar
                       var w21 = (0)
                       while w21 < 1 do
                        w21 = w21 + (1) /* inline */
                        var w22 = 0
                        while w22 < 1 do
                         w22 = w22 + 1
                         var w23 = 0

                         while w23 < 1 do
                          w23 = w23 + 1
// Fuzz vlozeny komentar
                          var w24 = (0)
                          while w24 < 1 do /* inline */
// Fuzz vlozeny komentar
                           w24 = w24 + 1 /* inline */
                           var w25 = 0
                           while w25 < (1) do
                            w25 = w25 + 1
                            var w26 = 0
                            while w26 < 1 do

                             w26 = w26 + 1 /* inline */
                             var w27 = (0) /* inline */
                             while w27 < 1 do
                              w27 = w27 + 1
                              var w28 = 0

                              while w28 < (1) do
// Fuzz vlozeny komentar
                               w28 = w28 + 1;var w29 = 0

                               while w29 < 1 do
                                w29 = w29 + (1)
// Fuzz vlozeny komentar
                                var w30 = (0)
                                while w30 < 1 do
                                 w30 = w30 + 1
                                 var w31 = (0)
                                 while w31 < 1 do
                                  w31 = w31 + (1)
                                  var w32 = 0 /* inline */
                                  while w32 < (1) do
                                   w32 = w32 + (1)

                                   var w33 = 0 /* inline */

                                   while w33 < 1 do
                                    w33 = w33 + (1)
// Fuzz vlozeny komentar
                                    var w34 = 0
// Fuzz vlozeny komentar

                                    while w34 < 1 do
                                     w34 = w34 + 1
                                     var w35 = 0

                                     while w35 < 1 do
// Fuzz vlozeny komentar
                                      w35 = w35 + (1) ;  var w36 = 0
                                      while w36 < 1 do
                                       w36 = w36 + (1)
                                       var w37 = 0
                                       while w37 < 1 do
                                        w37 = w37 + 1

                                        var w38 = (0)
                                        while w38 < 1 do /* inline */
                                         w38 = w38 + (1)
// Fuzz vlozeny komentar
                                         var w39 = (0)
// Fuzz vlozeny komentar
                                         while w39 < 1 do

                                          w39 = w39 + (1)

                                          print(1)
