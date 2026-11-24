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


## Generování kódu cílového jazyka

Generování díky návrhu předchozích stupňů překladače bylo výrazně zjednodušeno a 
prakticky  se  jedná  o  vylepšení  průchod  stromem,  kdy  jednotlivé  příkazy  se  generují 
pre-order  průchodem  stromu  a  výrazy  za  využití  post-order  průchod.  Maximum 
opakujících se konstrukcí bylo pro maximální přehlednost segregováno do separátních 
funkcí a modulů.

## Testování

Testování bylo realizováno pomocí testovacího lešení (tzv. harness), které zajišťovalo
korektní předání vstupních souborů, spuštění daného modulu a ověření jeho výstupu.
Testovací sady byly generovány pomocí tzv. mutačních fuzzerů, které vytvářely jak
syntakticky platné varianty výchozích vzorů (tzv. seedů – ručně napsaných programů ve
vstupním jazyce a ukázek převzatých z Moodlu předmětu), tak i jejich neplatné varianty
(vložení neplatného znaku do lexému, smazání závorky, odstranění deklarace proměnné
apod.). Všechny takto vygenerované programy byly ověřovány proti referenčnímu
překladači jazyka Scala. Díky tomuto algoritmickému přístupu bylo možné generovat
tisíce testů (teoreticky neomezené množství) bez zbytečného plýtvání tokeny pro LLM.