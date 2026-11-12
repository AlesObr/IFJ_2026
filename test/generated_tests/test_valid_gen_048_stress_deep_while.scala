// Fuzz vlozeny komentar
import ifj26.* /* inline */

@main def main(): Unit =
  var w0 = 0
  while /* c */ w0 < 1 do /* inline */
   w0 = w0 + 1 /* inline */

   var /**/ w1 = 0 /* inline */
   while w1 < 1 do /* inline */
    w1 = /**/ w1 + /* a /* b */ c */ 1
    var w2 = 0
    while w2 < 1 do

     w2 = w2 + 1 /* inline */
// Fuzz vlozeny komentar
     var w3 = 0
     while w3 < 1 do
      w3 = w3 + 1    
           
      var w4 = 0 /* inline */
      while w4 < 1 do
// Fuzz vlozeny komentar  
       w4 = w4 /* " */ + 1
      
// Fuzz vlozeny komentar

       var w5 = 0
       while w5 < 1 do
// Fuzz vlozeny komentar
        w5 = w5 + 1 /* inline */
        var w6 = 0 
        while w6 < 1 do
// Fuzz vlozeny komentar
         w6 = w6 /* c */ + 1

         var w7 = 0 
         while w7 /* a /* b */ c */ < 1 /* " */ do
          w7 /* c */ = /* c */ w7 + 1
          var w8 = 0
          while /* a /* b */ c */ w8 < 1 do
           w8 = w8 + /* a /* b */ c */ 1 /* inline */
           var w9 = 0
           while /**/ w9 < 1 do
            w9 = /* " */ w9 + 1
            var w10 = 0 /* inline */
            while w10 < 1 do
             w10 = w10 + /* " */ 1
             var w11 /* c */ = 0
// Fuzz vlozeny komentar
             while w11 < 1 do /* inline */
              w11 = w11 + 1
              var w12 = 0
// Fuzz vlozeny komentar
              while w12 < 1 do
               w12 = w12 + 1
               var w13 = /* c */ 0
               while w13 < 1 do
                w13 = w13 + 1
// Fuzz vlozeny komentar
                var w14 = 0    

                while /**/ w14 < 1 do /* inline */   
                 w14 = w14 + 1
                 var /* a /* b */ c */ w15 /* " */ = 0

                 while w15 < 1 do /* inline */

                  w15 = w15 + 1
                  var w16 = 0
                  while w16 < 1 do
// Fuzz vlozeny komentar
                   w16 /* " */ = w16 + 1
// Fuzz vlozeny komentar
                   var w17 = 0  
                   while w17 < 1 do
                    w17 = w17 + 1 
  
                    var w18 = 0
                    while w18 < 1 do
                     w18 = w18 + 1 /* inline */

                     var w19 /* " */ = 0    
                     while w19 < 1 do
                      w19 = w19 + 1
// Fuzz vlozeny komentar
                      var w20 = 0
                      while w20 < 1 /* " */ do
                       w20 = w20 + 1
                       var w21 = 0 /* inline */
// Fuzz vlozeny komentar
                       while w21 < 1 do
                        w21 = w21 + 1

                        var w22 = 0
                        while w22 < 1 do
                         w22 = w22 + 1
                         var w23 = 0
// Fuzz vlozeny komentar
                         while /* ) ( , = */ w23 < 1 do
                          w23 = w23 /* " */ + 1
         
                          var w24 = 0
                          while w24 < 1 do
                           w24 = w24 + /* ) ( , = */ 1
                           var w25 = 0   
// Fuzz vlozeny komentar
                           while /* ) ( , = */ w25 < 1 do
                            w25 = w25 + 1
                            var w26 = 0
                            while w26 < /* a /* b */ c */ 1 do
                             w26 = w26 + 1
                             var w27 = 0
                             while w27 < 1 do

                              w27 = w27 + 1

                              var w28 = 0
           
                              while /* " */ w28 < 1 do

                               w28 = w28 + 1

                               var w29 /* c */ = 0
                               while w29 < /* c */ 1 do    
                                w29 = w29 + 1
                                var w30 = 0
                                while w30 < 1 do
// Fuzz vlozeny komentar  
                                 w30 = w30 + 1 /* a /* b */ c */ /* inline */
                                 var w31 = 0 /* inline */
// Fuzz vlozeny komentar
                                 while w31 /* ) ( , = */ < 1 do
                                  w31 = w31 + /**/ 1 /* inline */
// Fuzz vlozeny komentar
                                  var w32 = 0
                                  while w32 < 1 do
                                   w32 /**/ = w32 + 1
                                   var /* " */ w33 = 0   
// Fuzz vlozeny komentar
                                   while w33 < /* c */ 1 do
                                    w33 = w33 + 1
                                    var w34 = 0
                                    while w34 < /* ) ( , = */ 1 do
                                     w34 = w34 + 1
                                     var w35 = 0
                                     while w35 < /* ) ( , = */ 1 do
                                      w35 = w35 /* " */ + /* ) ( , = */ 1
                                      var w36 = 0
                                      while w36 < 1 do
// Fuzz vlozeny komentar   
                                       w36 /* c */ = w36 + 1
// Fuzz vlozeny komentar
                                       var w37 = 0 /* inline */
// Fuzz vlozeny komentar    
        
                                       while w37 < 1 do
                                        w37 = /* " */ w37 + 1 
                                        var w38 = 0
// Fuzz vlozeny komentar
                                        while w38 < 1 do /* inline */
// Fuzz vlozeny komentar    
                                         w38 = w38 + 1
                                         var w39 = 0
                                         while w39 /* a /* b */ c */ < 1 do
                                          w39 /* " */ = w39 + /* " */ 1   
                                          print(1)   
