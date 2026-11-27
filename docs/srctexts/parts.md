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

Modul lexikální analýzy (lexer.c/.h) čte vstupní program ze standardního vstupu s podporou vracení načteného znaku (lexer_take(), lexer_give()) a generuje tokeny v hlavní funkci lexer_get_next_token() podle DKA viz kapitola 6. Tento byl pro realizaci implementace upraven. Kontextové odsazení bloků sleduje interní zásobník, obsah složených lexém (identifikátory, řetězcové literály) se ukládá přes vlastní knihovnu dstring a při výskytu první lexikální chyby překlad okamžitě selhává s kódem 1 bez pokusu o zotavení.

## Syntaktická

Syntaktický analyzátor kombinuje rekurzivní sestup podle LL-gramatiky (viz kapitola 7) a precedenční analýzu zdola nahoru (pomocí precedenční tabulky), pro výrazy. Chyby a uvolňování paměti rozpracovaných podstromů řeší makra (například GET_TOKEN() pro kontrolu výstupu lexeru). Funkce komunikují přes strukturu ParserStatus. Parser postupně vkládá jednotlivé uzly, které jsou řešeny pomocí značených unií (tzv. tagged union). Modul byl od počátku testován přímo s hotovým lexikálním analyzátorem.


## Tabulka symbolů

základní datovou strukturou pro sémantickou analýzu je tabulka symbolů (symtable), která byla implementovaná jako hashovací tabulka s otevřenou adresací
(varianta TRP-IZP).  Definuje základní operace pro vyhledávání, vkládání a odstraňování tabulky. Sémantický analyzátor ji využívá na dvou úrovních: jednak jako globální tabulku funkcí (ftable) a jednak pro jednotlivé úrovně zásobníku rozsahů platnosti (scope_table) pro lokální proměnné.

## Sémantická analýza

Sémantická analýza probíhá nad AST ve dvou průchodech. V prvním průchodu registruje všechny vestavěné i uživatelské funkce do globální tabulky funkcí ftable a sleduje stav jejich analýzy (nezačala, probíhá, dokončena), což umožňuje volat funkce před jejich definicí, řešit rekurzi i odvozovat implicitní návratové typy (u rekurzivní funkce bez deklarovaného typu hlásí chybu).
Ve druhém průchodu kontroluje těla funkcí podle pravidel v type_rules.c za pomoci zásobníku rozsahů scope_table, jehož vkládání a odebírání při vstupu a výstupu z bloků přirozeně zajišťuje zastiňování proměnných. Pro zjednodušení generátoru kódu zároveň obohacuje uzly AST o jedinečná ID proměnných v rámci funkce (od 1 pro parametry), výsledné typy výrazů (expr_types) a příznaky implicitní konverze (to_double), přičemž veškeré chyby propaguje návratovým typem Error s přesnou pozicí v kódu.


## Generování kódu cílového jazyka

Díky návrhu předchozích fází překladače je generování cílového kódu (code_gen.c/.h) založeno na přímém průchodu obohaceným abstraktním syntaktickým stromem (AST). Řídicí konstrukce a jednotlivé příkazy se zpracovávají pre-order průchodem, zatímco vyhodnocování výrazů probíhá post-order průchodem, který svým chováním odpovídá zásobníkovému vyhodnocování výrazů. Opakující se instrukční sekvence a vestavěné funkce jsou pro přehlednost vyčleněny do samostatných pomocných funkcí.

## Testování

Testování bylo realizováno pomocí testovacího lešení (tzv. harness), které zajišťovalo korektní předání vstupních souborů, spuštění daného modulu a ověření jeho výstupu. Testovací sady byly generovány pomocí tzv. mutačních fuzzerů, které vytvářely jak syntakticky platné varianty výchozích vzorů (tzv. seedů – ručně napsaných programů ve vstupním jazyce a ukázek převzatých z Moodlu předmětu), tak i jejich neplatné varianty (vložení neplatného znaku do lexému, smazání závorky, odstranění deklarace proměnné apod.). Všechny takto vygenerované programy byly ověřovány proti referenčnímu překladači jazyka Scala. Díky tomuto algoritmickému přístupu bylo možné generovat tisíce testů (teoreticky neomezené množství) bez zbytečného plýtvání tokeny pro LLM.