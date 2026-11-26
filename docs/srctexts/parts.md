# Lexer

- Token.h
- Token.c

Uložení tokenu a práce nad ním

-------
- Lexer.c
- Lexer.h
Stavový automat, vrstva pro správu odszaení (stack), stejně tak na l/r paren
-------
- Dstring.c
- Dstring.h

Elegatní správa textových řetězců pro zajištění teoreticky nekončené délky stringu při načítání tokenu

## Lexikální analýza

Modul lexikální analýzy (lexer.c/.h) čte vstupní program ze standardního vstupu s podporou vracení načteného znaku (lexer_take(), lexer_give()) a generuje tokeny v hlavní funkci lexer_get_next_token() podle DKA viz kapitola 6. Kontextové odsazení bloků sleduje interní zásobník, obsah složených lexém (identifikátory, řetězcové literály) se ukládá přes vlastní knihovnu dstring a při výskytu první lexikální chyby překlad okamžitě selhává s kódem 1 bez pokusu o zotavení.

## Syntaktická analýza

Parser vychází z LL-gramatiky[2], která od začátku zahrnovala všechna plánovaná rozšíření a během vývoje byla upravena pro jedno dodatečné. Řízení toku zajišťují makra obalující chybově náchylná volání, především GET_TOKEN, který načte a ověří token z lexeru a při selhání uvolní paměť podprogramu. Funkce spolu komunikují přes strukturu ParserStatus (chybová zpráva typu Error, příznak úspěchu a ukazatel na uzel AST); při chybě se dosud vytvořená část stromu makry dealokuje. Testování probíhalo s téměř hotovým lexerem místo mockupu a AST[3],[4] tvoří uzly s unií přístupnou podle výčtového typu, přičemž výrazy se zpracovávají Prattovým parserem, který si díky číselným vahám priorit operátorů vystačí s několika jednoduchými funkcemi.


## Generování kódu cílového jazyka

Díky návrhu předchozích fází překladače je generování cílového kódu (code_gen.c/.h) založeno na přímém průchodu obohaceným abstraktním syntaktickým stromem (AST). Řídicí konstrukce a jednotlivé příkazy se zpracovávají pre-order průchodem, zatímco vyhodnocování výrazů probíhá post-order[5] průchodem. Opakující se instrukční sekvence a vestavěné funkce jsou pro přehlednost vyčleněny do samostatných pomocných funkcí.

## Testování

Testování bylo realizováno pomocí testovacího lešení (tzv. harness), které zajišťovalo korektní předání vstupních souborů, spuštění daného modulu a ověření jeho výstupu. Testovací sady byly generovány pomocí tzv. mutačních fuzzerů[1], které vytvářely jak syntakticky platné varianty výchozích vzorů (tzv. seedů – ručně napsaných programů ve vstupním jazyce a ukázek převzatých z Moodlu předmětu), tak i jejich neplatné varianty (vložení neplatného znaku do lexému, smazání závorky, odstranění deklarace proměnné apod.). Všechny takto vygenerované programy byly ověřovány proti referenčnímu překladači jazyka Scala. Díky tomuto algoritmickému přístupu bylo možné generovat tisíce testů (teoreticky neomezené množství) bez zbytečného plýtvání tokeny pro LLM.