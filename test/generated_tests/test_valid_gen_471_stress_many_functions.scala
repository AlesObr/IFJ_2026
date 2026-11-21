import ifj26.* /* inline */
// Fuzz vlozeny komentar

def f0(x: Int): Int = x + 0
def f1(x: Int): Int = x + 1
// Fuzz vlozeny komentar
def f2(x: Int): Int = x + 2
// Fuzz vlozeny komentar
def f3(x: Int): Int = x /* c */ + 3
def f4(x: Int): Int /* c */ = x + 4
def f5(x: Int): Int = x + 5
def f6(x: Int): Int = x /* a /* b */ c */ + 6
// Fuzz vlozeny komentar
def f7(x: /* a /* b */ c */ Int): Int = x + 7 /* inline */
def f8(x: Int): Int = x + 8
def f9(x: Int): Int = /* ) ( , = */ x + 9
def f10(x: /* ) ( , = */ Int): Int = x + 10

def f11(x: Int): Int = x + 11
def f12(x: Int): /* c */ Int = x + /* c */ 12
def f13(x: Int): Int = x + /* " */ 13
def f14(x: /* a /* b */ c */ Int): Int = x + 14 /* c */ /* inline */
def f15(x: Int): Int = x + 15
// Fuzz vlozeny komentar
def f16(x: Int): Int = x /**/ + 16
def f17(x: Int): Int = x + 17
def /* ) ( , = */ f18(x: Int): Int = x + 18
def f19(x: Int): Int = x + 19
// Fuzz vlozeny komentar
def f20(x: Int): Int = x + 20
def f21(x: Int): Int = x + 21

def f22(x: Int): Int = x + 22
def f23(x: Int): /* ) ( , = */ Int = x + 23
def f24(x: Int): Int = x + 24
def /* a /* b */ c */ f25(x: Int): Int /* ) ( , = */ = x + 25
def /**/ f26(x: Int): Int = x + 26
def f27(x: Int): Int = x + 27 /* inline */

def f28(x: Int): Int = x /* ) ( , = */ + 28
def f29(x: Int): Int = x + 29 /* inline */
def f30(x: Int): Int = x + 30
def f31(x: Int): Int = x + 31 /* inline */

def f32(x: Int): Int = x + 32
def f33(x: Int): Int = x + 33
def f34(x: Int): Int = x + 34
def f35(x: Int): /* c */ Int = x + 35
def f36(x: Int): Int = x + /* c */ 36

def f37(x: Int): Int = x + 37
// Fuzz vlozeny komentar
def f38(x: Int): Int = x + 38
def f39(x: Int): Int = x + /**/ 39
def f40(x: Int): Int = x + /* a /* b */ c */ 40 /* inline */
def f41(x: Int): Int = x + 41
def f42(x: Int): Int = x + 42
def f43(x: Int): Int = x + 43

def f44(x: Int): Int = x + 44
def f45(x: /* ) ( , = */ Int): Int = x + 45
// Fuzz vlozeny komentar
def f46(x: Int): Int = x + 46
def f47(x: Int): Int = x + 47
def f48(x: Int): Int = x + 48
def /* a /* b */ c */ f49(x: Int): Int = x + 49 /* inline */

def f50(x: Int): Int /* c */ = x + 50
def f51(x: Int): Int = x + 51
def f52(x: Int): Int = x + 52
def f53(x: Int): Int = x + 53
def f54(x: Int): Int = /**/ x + 54 /* inline */
def f55(x: /* a /* b */ c */ Int): Int = x + 55

def /**/ f56(x: Int): Int = x + 56 /* inline */
def f57(x: Int): Int = x /* " */ + 57
def f58(x: Int): Int = x /* a /* b */ c */ + 58
def f59(x: Int): Int = x + 59
def f60(x: Int): /* c */ Int = x + 60

def f61(x: Int): Int = x + 61 /* inline */
def f62(x: Int): Int = /* " */ x + 62 /* inline */
def f63(x: Int): Int = x + 63

def f64(x: Int): Int = x + 64 /* inline */
def f65(x: Int): Int = x + 65
def f66(x: Int): Int = x + 66
def /* a /* b */ c */ f67(x: Int): Int = x + 67
def f68(x: Int): Int = /* ) ( , = */ x + 68
def f69(x: Int): Int /**/ = x + 69
def f70(x: Int): Int = x + 70 /* inline */

def /* a /* b */ c */ f71(x: Int): Int = x /* ) ( , = */ + 71
def /* c */ f72(x: Int): Int = x + 72
def f73(x: Int): Int = x + 73
def f74(x: Int): /* ) ( , = */ Int = x /* " */ + 74
def f75(x: Int): Int = x + 75
def f76(x: Int): Int = x /* a /* b */ c */ + 76
def f77(x: Int): Int = x + 77

def f78(x: Int): Int = x + 78
def f79(x: Int): /* " */ Int /* c */ = x + /**/ 79
def f80(x: Int): Int = x + 80
def f81(x: Int): Int = x + /* ) ( , = */ 81
// Fuzz vlozeny komentar
def f82(x: Int): Int = x + 82
def f83(x: Int): Int = x + 83
// Fuzz vlozeny komentar
def f84(x: Int): Int = x + 84
def f85(x: Int): Int = /**/ x + 85
def f86(x: Int): Int = x + 86
// Fuzz vlozeny komentar
def f87(x: Int): /* c */ Int = x + /* a /* b */ c */ 87
def f88(x: Int): Int = x + 88
def f89(x: Int): Int = x + 89
def f90(x: /* ) ( , = */ Int): Int = x + 90 /* inline */
def f91(x: Int): Int = x + 91

def f92(x: Int): Int = x + 92
def f93(x: Int): Int = x + 93
def f94(x: Int): Int = x + 94 /* inline */

def f95(x: Int): Int = x + 95
def f96(x: /* c */ Int): Int /* c */ = x + 96
def f97(x: Int): Int = x + 97
def /**/ f98(x: Int): Int /* " */ = x + 98
def f99(x: Int): Int = x + 99
def f100(x: Int): Int = x + 100
// Fuzz vlozeny komentar
def f101(x: Int): Int = x + 101
def f102(x: Int): Int = x + 102
// Fuzz vlozeny komentar
def f103(x: Int): Int = x + 103 /* inline */

def f104(x: Int): Int = x + 104
def f105(x: /* a /* b */ c */ Int): Int = x + 105
def f106(x: Int): Int = x + 106
// Fuzz vlozeny komentar
def f107(x: Int): Int = x + 107
def f108(x: Int): /* a /* b */ c */ Int = /* " */ x /* ) ( , = */ + 108
def f109(x: Int): Int = x + 109
def f110(x: Int): Int /* c */ = x + 110
def f111(x: Int): Int = x + 111
def f112(x: Int): Int = x + 112

def f113(x: Int): Int = x + 113
// Fuzz vlozeny komentar
def f114(x: Int): Int = x + 114
// Fuzz vlozeny komentar
def f115(x: Int): Int = x + 115
def f116(x: Int): Int /* ) ( , = */ = x + 116 /* inline */
def f117(x: Int): Int = x + 117
def f118(x: Int): Int = x + 118
def f119(x: /* c */ Int): Int = x + 119
def f120(x: Int): Int /* ) ( , = */ = x + 120
def f121(x: Int): Int = x + 121
def f122(x: /* a /* b */ c */ Int): Int = x + 122
def f123(x: Int): Int = x + 123 /* inline */
def f124(x: Int): Int = x + 124
def f125(x: Int): Int = x + 125
// Fuzz vlozeny komentar
def f126(x: Int): Int = x + 126
def f127(x: Int): Int = x + 127

def f128(x: Int): Int = x + 128
def f129(x: Int): Int = x /* ) ( , = */ + 129
def f130(x: Int): Int = x + 130 /* inline */
def /* a /* b */ c */ f131(x: Int): Int = x + 131
def f132(x: Int): Int /* c */ = x + 132
// Fuzz vlozeny komentar
def f133(x: Int): Int = x + 133
def f134(x: Int): Int = x + 134
def f135(x: Int): Int /* c */ = x + 135
// Fuzz vlozeny komentar
def f136(x: Int): Int = x + 136
def f137(x: Int): Int = x + 137 /* inline */
def f138(x: Int): Int = x + 138
def /**/ f139(x: Int): Int = x + 139

def f140(x: Int): Int = x + 140
def f141(x: Int): Int = x + 141
def f142(x: Int): Int = x + 142
def f143(x: Int): Int = x + 143
def f144(x: Int): Int = x + 144 /* inline */
def f145(x: Int): Int = x + 145 /* inline */
def f146(x: Int): Int = x + /* ) ( , = */ 146
def f147(x: Int): Int = /* " */ x + 147 /* inline */
def /* " */ f148(x: Int): Int = x + 148 /* inline */

def f149(x: Int): Int = x + 149
def f150(x: Int): Int = x + 150 /* inline */
def f151(x: Int): Int = x + /**/ 151
def f152(x: Int): Int = x + 152
def f153(x: Int): Int = x + 153
// Fuzz vlozeny komentar
def f154(x: Int): Int = x + 154
def f155(x: Int): Int = x + /* ) ( , = */ 155
def f156(x: /* " */ Int): /* " */ Int = x + 156
def f157(x: Int): Int = x + 157
def /* ) ( , = */ f158(x: Int): Int = /* " */ x + 158
def f159(x: /**/ Int): Int = x + 159
def f160(x: Int): Int = x + 160
// Fuzz vlozeny komentar
def f161(x: Int): Int = x + 161
def f162(x: Int): Int = x + 162

def /* ) ( , = */ f163(x: Int): /* " */ Int /* a /* b */ c */ = x + 163

def f164(x: Int): Int = x + 164
def f165(x: /* a /* b */ c */ Int): Int = x + 165
def f166(x: Int): Int = x + 166
def f167(x: Int): Int = x + 167
def f168(x: Int): /**/ Int = x + 168
def /* c */ f169(x: /* " */ Int): Int = x + 169

def f170(x: Int): Int = x + 170
def f171(x: Int): Int = /* a /* b */ c */ x + 171
def f172(x: Int): Int = x + 172
def f173(x: /* a /* b */ c */ Int): Int = x + 173 /* inline */
def f174(x: Int): Int = /* " */ x + 174
def /* " */ f175(x: Int): Int = x + 175
def f176(x: Int): Int = x + 176 /* inline */
def f177(x: Int): Int = x + 177
def f178(x: Int): Int = x + 178
def f179(x: /* c */ Int): Int = x /* ) ( , = */ + 179 /* inline */
def f180(x: Int): Int = x + 180
def f181(x: Int): Int = x + 181
def f182(x: Int): Int = x + 182
// Fuzz vlozeny komentar
def f183(x: Int): Int = x + 183
def f184(x: Int): Int /* a /* b */ c */ = x + 184
def f185(x: Int): Int = x + 185
def f186(x: Int): Int = x + 186
// Fuzz vlozeny komentar
def f187(x: Int): Int = x + 187 /* inline */
def /* ) ( , = */ f188(x: Int): Int = x + 188
// Fuzz vlozeny komentar
def f189(x: Int): Int = x + 189
def f190(x: Int): Int = x /* a /* b */ c */ + 190
def f191(x: Int): Int = x + 191 /* inline */
def f192(x: Int): Int = x + 192
def /* c */ f193(x: Int): Int = x /* c */ + 193
def f194(x: Int): Int = x + 194 /* inline */
def f195(x: Int): Int = x + 195
def f196(x: Int): Int /* a /* b */ c */ = x + 196
def /* a /* b */ c */ f197(x: Int): Int = x + 197 /* inline */
def f198(x: Int): Int = x + 198
// Fuzz vlozeny komentar

def f199(x: Int): Int = x + 199 /* inline */
def f200(x: Int): Int = x + 200
def f201(x: Int): Int = x + /* c */ 201 /* inline */
def f202(x: Int): Int /**/ = x + 202 /* inline */
def f203(x: Int): Int = x + 203 /* ) ( , = */ /* inline */
def f204(x: Int): Int = x + 204

def f205(x: Int): Int = x + 205 /* inline */
// Fuzz vlozeny komentar
def f206(x: Int): Int = x + 206
def f207(x: Int): Int = x + /* c */ 207
def f208(x: Int): Int = x + 208
def f209(x: Int): Int = x + 209
def f210(x: Int): Int = x + 210
// Fuzz vlozeny komentar
def f211(x: Int): Int = x + 211 /* inline */
def f212(x: Int): Int /* " */ = x + 212
def f213(x: Int): Int = /* c */ x + 213
def f214(x: Int): Int /* ) ( , = */ = x + /* a /* b */ c */ 214
// Fuzz vlozeny komentar
def f215(x: /* ) ( , = */ Int): Int = x + 215
def f216(x: Int): Int = x + 216
def f217(x: Int): Int = x + 217
def f218(x: Int): Int = x + /* ) ( , = */ 218
def f219(x: Int): Int = x + 219
def f220(x: Int): Int = /* ) ( , = */ x + 220
def f221(x: Int): Int = x + 221
def f222(x: Int): /* ) ( , = */ Int = x /* " */ + 222
def /* c */ f223(x: Int): Int = x + 223 /* inline */
def f224(x: Int): Int = x /* a /* b */ c */ + 224 /* inline */
def f225(x: Int): Int = x + 225 /* inline */
def f226(x: Int): Int = x + 226

def f227(x: Int): Int = x + 227
def f228(x: Int): Int = x + 228

def f229(x: Int): Int = x + 229
def f230(x: Int): Int = x + 230 /* inline */
// Fuzz vlozeny komentar
def f231(x: Int): Int = x + 231
def f232(x: Int): Int = /**/ x + /* a /* b */ c */ 232
def f233(x: Int): Int = x + 233
def f234(x: Int): Int = x + 234 /* inline */
def /* ) ( , = */ f235(x: Int): Int = x + 235 /* inline */
def f236(x: Int): Int = x + 236

def f237(x: Int): Int = x + 237
def f238(x: Int): Int = x + 238

def f239(x: Int): Int = /* c */ x + 239
def f240(x: Int): Int = x + 240
def f241(x: Int): Int = x + 241
def f242(x: Int): Int = x + 242
def f243(x: Int): Int = x + 243
def f244(x: Int): Int = x + 244
def f245(x: Int): Int = x + 245 /* a /* b */ c */ /* inline */
def f246(x: Int): Int = x + 246
// Fuzz vlozeny komentar
def f247(x: Int): Int /* ) ( , = */ = x + 247
def /* a /* b */ c */ f248(x: Int): Int = x + 248
def f249(x: Int): Int /* ) ( , = */ = x + 249
def f250(x: Int): Int = /* a /* b */ c */ x + 250
def f251(x: Int): Int = x + 251
def f252(x: Int): Int = x + 252
def f253(x: Int): Int = /* c */ x + 253
def f254(x: Int): Int = x + 254
// Fuzz vlozeny komentar
def f255(x: Int): Int = x + 255
def f256(x: /* a /* b */ c */ Int): Int /* " */ = x + 256
def f257(x: Int): Int = x + 257
def f258(x: Int): /* ) ( , = */ Int = x + 258
def f259(x: Int): Int = x + 259
def f260(x: Int): Int = x /* a /* b */ c */ + 260
def /* c */ f261(x: Int): /* c */ Int = x + 261
def f262(x: Int): Int = x + 262
def f263(x: Int): Int = x /* " */ + 263
def f264(x: Int): Int = x + 264 /* inline */
def f265(x: Int): Int = x + 265
// Fuzz vlozeny komentar
def f266(x: Int): Int = x + 266
// Fuzz vlozeny komentar
def f267(x: Int): /**/ Int = /* c */ x + 267
def f268(x: Int): Int = x + 268
def f269(x: Int): Int = x + 269
def f270(x: Int): Int = /* c */ x + 270
// Fuzz vlozeny komentar
def f271(x: Int): Int = x + 271
def f272(x: Int): Int = x + 272 /* inline */
def /* c */ f273(x: Int): Int = x + 273
// Fuzz vlozeny komentar
def /* c */ f274(x: Int): Int = x + /* " */ 274
def f275(x: Int): Int = x + 275
def f276(x: Int): /* a /* b */ c */ Int = x + 276
def f277(x: Int): /**/ Int = x + 277
// Fuzz vlozeny komentar
def f278(x: Int): Int = x + 278
def f279(x: Int): Int = x /**/ + 279 /* inline */
def /* c */ f280(x: Int): Int = x + 280
// Fuzz vlozeny komentar
def f281(x: Int): Int = x + 281
// Fuzz vlozeny komentar
def f282(x: Int): Int = x + 282
def f283(x: Int): Int = x + /* " */ 283
// Fuzz vlozeny komentar
def f284(x: Int): Int = x + 284
def f285(x: Int): Int = x /* a /* b */ c */ + 285

def f286(x: Int): Int = x + 286
// Fuzz vlozeny komentar
def f287(x: Int): Int = x + 287
// Fuzz vlozeny komentar
def f288(x: Int): Int = x + 288
def f289(x: Int): Int /* ) ( , = */ = x + 289
def f290(x: Int): Int = x + 290
def f291(x: Int): Int = x + 291
def f292(x: Int): Int = x + 292
def f293(x: Int): Int = x + 293
// Fuzz vlozeny komentar
def f294(x: Int): Int = x + 294
def f295(x: Int): Int = x + /* c */ 295
def f296(x: Int): Int = x + 296
def /* a /* b */ c */ f297(x: Int): /* a /* b */ c */ Int = x + 297 /* inline */
def f298(x: Int): Int = x + 298
def f299(x: Int): Int = x + 299
def f300(x: Int): Int = x + 300 /* inline */
def f301(x: Int): Int = x + 301
// Fuzz vlozeny komentar
def f302(x: Int): Int = x + 302
def /**/ f303(x: Int): Int = x + 303
def f304(x: Int): Int = x + 304
// Fuzz vlozeny komentar
def f305(x: /* c */ Int): /**/ Int /* " */ = x + 305
def f306(x: /**/ Int): Int = x + 306
def f307(x: Int): Int = x + 307
def f308(x: Int): /**/ Int = x + 308 /* ) ( , = */ /* inline */
// Fuzz vlozeny komentar

def f309(x: Int): Int = x + 309

def /* a /* b */ c */ f310(x: Int): Int = x + 310
// Fuzz vlozeny komentar
def f311(x: Int): Int = x /* a /* b */ c */ + 311
def f312(x: Int): Int = x + 312
def f313(x: Int): /**/ Int = x + 313
def f314(x: Int): Int = /* ) ( , = */ x + 314 /* inline */
def f315(x: Int): /* ) ( , = */ Int = x + 315
def f316(x: Int): Int = x /* " */ + 316
// Fuzz vlozeny komentar
def f317(x: Int): Int = x + 317
def f318(x: Int): Int = x + 318
def f319(x: Int): Int = x + 319
def f320(x: Int): Int = x + 320 /* inline */
def f321(x: Int): Int = x + 321
def f322(x: Int): Int /* " */ = x + /**/ 322
def f323(x: Int): Int = x + 323
// Fuzz vlozeny komentar

def f324(x: Int): /* a /* b */ c */ Int = x + 324
def f325(x: Int): Int = x + 325
def f326(x: Int): Int = x + 326
def f327(x: Int): Int = x + 327
def f328(x: /* c */ Int): Int = x + 328
// Fuzz vlozeny komentar
def /**/ f329(x: Int): Int = x + 329 /* inline */
def /* c */ f330(x: Int): Int = x + 330
def f331(x: Int): Int = x + 331
def f332(x: Int): Int = x + 332
def f333(x: Int): Int = x + 333
def f334(x: Int): Int = x + 334
// Fuzz vlozeny komentar
def f335(x: Int): Int = /* ) ( , = */ x + 335
def f336(x: Int): Int = x + 336
def f337(x: Int): Int = x + 337
def f338(x: Int): Int = x + 338 /* inline */
def f339(x: Int): Int /* " */ = x /* c */ + 339
// Fuzz vlozeny komentar
def /* " */ f340(x: Int): Int = x + /* ) ( , = */ 340 /* inline */
def f341(x: /* ) ( , = */ Int): Int = x + 341
def f342(x: Int): Int = x + 342
def f343(x: Int): Int = x + 343
def f344(x: Int): Int = x /* a /* b */ c */ + 344
def f345(x: Int): Int = x + 345
def f346(x: Int): /* ) ( , = */ Int = x /* " */ + 346
def f347(x: Int): Int /* ) ( , = */ = x + /* " */ 347
def f348(x: Int): Int = /**/ x + 348
def f349(x: /* c */ Int): Int = /**/ x + 349
def f350(x: Int): Int = x + 350

def f351(x: Int): Int = x + 351
// Fuzz vlozeny komentar
def f352(x: Int): Int = x + 352
def f353(x: Int): Int = x + 353
def f354(x: Int): Int = x + 354
def f355(x: Int): Int = x + 355
def /**/ f356(x: Int): Int = x + 356
def f357(x: Int): /* c */ Int = x + 357 /* inline */
def f358(x: Int): Int = x + 358
def f359(x: Int): /* ) ( , = */ Int = x + 359 /* " */ /* inline */
def f360(x: Int): Int = x + 360 /* inline */
def f361(x: Int): Int = x + 361
def /* " */ f362(x: Int): /**/ Int = x + 362 /* inline */
def /* a /* b */ c */ f363(x: Int): Int = /* a /* b */ c */ x + 363
def f364(x: Int): Int = x + /* a /* b */ c */ 364
def f365(x: Int): Int = x + 365
def f366(x: Int): Int = x + 366
def f367(x: Int): Int = x + 367
def f368(x: Int): Int /* " */ = x + 368
// Fuzz vlozeny komentar
def f369(x: Int): Int /* a /* b */ c */ = x + 369
def f370(x: Int): Int = x + /**/ 370
def f371(x: Int): Int = x + 371
def /* c */ f372(x: Int): /* c */ Int = x + 372
def f373(x: Int): Int = x + 373 /* inline */
def /* " */ f374(x: Int): Int = x + 374
def f375(x: Int): Int = x + 375
def f376(x: Int): Int = /**/ x + 376

def f377(x: Int): Int /* a /* b */ c */ = /* " */ x + 377
def f378(x: Int): Int = x + 378
def f379(x: /**/ Int): Int = x + 379

def /* a /* b */ c */ f380(x: Int): Int = x + 380
def f381(x: Int): /* " */ Int = x + 381
def f382(x: Int): Int = x + 382
def f383(x: Int): Int = x + 383 /* inline */
def /* a /* b */ c */ f384(x: Int): Int = x + 384
def f385(x: Int): /**/ Int = x /**/ + 385
def f386(x: Int): Int = x /* " */ + 386
def f387(x: Int): Int = x + 387
// Fuzz vlozeny komentar
def f388(x: Int): Int = x + 388
// Fuzz vlozeny komentar
def f389(x: Int): Int = x + 389
def f390(x: Int): Int = x + 390
def f391(x: /* a /* b */ c */ Int): Int = x + 391
def f392(x: Int): Int = x + 392 /* inline */
// Fuzz vlozeny komentar
def f393(x: Int): Int = x + 393
def f394(x: Int): Int = x + 394
def f395(x: Int): Int = /* c */ x + 395 /* inline */
def f396(x: Int): Int = x + 396 /* inline */
def f397(x: Int): Int = x + 397
def f398(x: Int): Int = x + 398
def /* a /* b */ c */ f399(x: Int): Int = x + 399
def f400(x: Int): Int = x + 400
// Fuzz vlozeny komentar
def /* a /* b */ c */ f401(x: Int): Int = x + 401
def f402(x: Int): Int = x + /* ) ( , = */ 402
def f403(x: Int): Int = x + 403
def f404(x: Int): Int = x + 404
def f405(x: Int): Int = x + 405
def f406(x: Int): Int = x + 406
def f407(x: Int): Int = x + 407
def f408(x: Int): Int = x /* a /* b */ c */ + 408 /* inline */
def f409(x: Int): Int = x + 409
def f410(x: Int): Int = /* " */ x + 410
def f411(x: Int): Int /* c */ = x + 411

def f412(x: Int): Int = x + 412

def f413(x: Int): Int = x + 413
def f414(x: Int): Int = x + /* a /* b */ c */ 414
def f415(x: Int): Int = x + 415
def f416(x: Int): Int = x + 416
def f417(x: Int): Int = x + /* c */ 417
// Fuzz vlozeny komentar
def f418(x: Int): Int = x + 418
def f419(x: /* c */ Int): /* c */ Int = x + 419
def f420(x: Int): /* c */ Int = x + 420

def f421(x: Int): Int = x + 421
def f422(x: /* c */ Int): Int = x + 422
def f423(x: Int): Int = x + 423
// Fuzz vlozeny komentar
def f424(x: Int): Int = x /* c */ + 424
// Fuzz vlozeny komentar
def f425(x: Int): Int = x + 425 /* inline */
def f426(x: Int): Int = x + 426
def f427(x: Int): Int = x + /* a /* b */ c */ 427
def f428(x: Int): Int = x + 428
def f429(x: Int): Int = x + /* ) ( , = */ 429
// Fuzz vlozeny komentar
def f430(x: Int): Int = x + 430
def f431(x: Int): Int = x + 431
def f432(x: Int): Int /* ) ( , = */ = x + 432
def f433(x: Int): Int = x + 433 /* inline */
def f434(x: Int): Int = x + 434
def f435(x: /**/ Int): Int = x + 435
// Fuzz vlozeny komentar
def f436(x: Int): Int = x + 436
// Fuzz vlozeny komentar
def f437(x: Int): Int = /* a /* b */ c */ x + 437

def f438(x: Int): Int = x + 438
def f439(x: Int): Int = /* ) ( , = */ x /* a /* b */ c */ + /* " */ 439
def f440(x: Int): Int = /* a /* b */ c */ x + 440
def f441(x: Int): Int = x + 441
def f442(x: Int): Int = x + 442
def f443(x: Int): Int /**/ = x + 443
def f444(x: Int): /* " */ Int = x + 444
def f445(x: /**/ Int): Int = x + 445 /* inline */
def /**/ f446(x: Int): Int = x + 446
// Fuzz vlozeny komentar
def f447(x: Int): Int = x + 447
def f448(x: Int): Int = x /* " */ + 448
def f449(x: Int): Int = x + 449
def f450(x: Int): Int = x + 450
def f451(x: Int): Int = x /* a /* b */ c */ + 451 /* inline */
def f452(x: Int): Int = x + 452
def f453(x: Int): Int = x + 453
def f454(x: Int): Int = x + 454
// Fuzz vlozeny komentar
def f455(x: Int): Int = x /* a /* b */ c */ + 455
def f456(x: Int): /* " */ Int = x + 456
def f457(x: Int): Int = x + 457

def f458(x: Int): Int = x + 458
def f459(x: /* ) ( , = */ Int): Int = x + 459
def f460(x: Int): Int = x + 460

def f461(x: Int): Int = x + 461

def f462(x: Int): Int = x + 462
def /* a /* b */ c */ f463(x: Int): Int = x + 463 /* inline */
// Fuzz vlozeny komentar
def f464(x: Int): Int = x + 464
def f465(x: /* ) ( , = */ Int): Int = x + 465
def f466(x: Int): Int = x + 466
def f467(x: Int): Int = x + 467

def f468(x: Int): Int = x + 468 /* inline */
// Fuzz vlozeny komentar
def f469(x: Int): Int /**/ = x + 469
def f470(x: Int): Int = /* a /* b */ c */ x + /* ) ( , = */ 470
// Fuzz vlozeny komentar
def f471(x: Int): Int = x + 471 /* inline */
def f472(x: Int): /* " */ Int = x /**/ + 472
def f473(x: Int): Int = x + 473 /* inline */
def f474(x: /* a /* b */ c */ Int): Int = x + 474
def f475(x: Int): Int = x + 475
def f476(x: Int): Int /* c */ = x + 476
def f477(x: Int): Int = x + 477
def f478(x: Int): Int = x + 478 /* inline */
def /* a /* b */ c */ f479(x: Int): Int = x + 479
def f480(x: Int): Int /* " */ = x + 480
def f481(x: Int): Int = x + 481
def f482(x: Int): Int = x + 482
def f483(x: Int): Int = /**/ x + 483
def f484(x: Int): Int = /* ) ( , = */ x + 484

def f485(x: Int): Int = x + 485
def f486(x: Int): Int = x + /* " */ 486
def f487(x: Int): Int = x + 487
def f488(x: Int): Int = x + 488
def f489(x: Int): Int = x /* c */ + 489
def f490(x: /* " */ Int): /**/ Int = x + 490
def f491(x: Int): /* c */ Int = x + 491
def f492(x: Int): /* ) ( , = */ Int = x + /* a /* b */ c */ 492
// Fuzz vlozeny komentar
def f493(x: Int): Int = x + 493
def f494(x: Int): Int = x + 494
def f495(x: Int): Int = x + 495
def f496(x: Int): Int = x + 496
def f497(x: Int): Int = x + 497
def /**/ f498(x: Int): Int /**/ = /**/ x + 498
def f499(x: Int): /* " */ Int = x + 499 /* inline */

@main def main(): Unit =
  print(f499(1))
