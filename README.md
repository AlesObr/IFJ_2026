# Překladač jazyka IFJ26

Implementace překladače imperativního jazyka **IFJ26** do cílového mezikódu **IFJcode26**, vytvořená v rámci předmětů Formální jazyky a překladače (IFJ) a Algoritmy (IAL) na FIT VUT (akademický rok 2026/2027).

---

## Architektonické principy a standardy

Implementace striktně dodržuje formální specifikaci zadání a zásady návrhu překladačů:

* **Lexikální analýza:** Deterministický konečný automat (DKA) zpracovávající vstupní proud znaků na silně typované tokeny.
* **Syntaktická analýza:** Hybridní syntaktický analyzátor kombinující rekurzivní sestup pro LL gramatiku a precedenční syntaktickou analýzu s tabulkou operátorů pro výrazy.
* **Tabulka symbolů:** Zapouzdřený abstraktní datový typ (ADT) implementovaný pomocí vyhledávací struktury dle zvolené varianty.
* **Správa paměti:** Důraz na korektní dealokaci paměti; verifikace pomocí nástroje Valgrind ve standardních i chybových větvích programu.
* **Přenositelnost:** Dodržení standardu ISO C11 a kompatibilita s referenčním překladovým prostředím na serveru Merlin.

---

## Zásady vývoje a týmová spolupráce

1. **Jednotné formátování:** Konzistence kódu je vynucována nástrojem `clang-format` dle projektové konfigurace.
2. **Pre-commit kontroly:** Lokální git hooky ověřují formátování, hlavičky a základní hygienu kódu před vytvořením commitu.
3. **Chráněná hlavní větev:** Přímé commity do větve `main` jsou zakázány; veškeré úpravy prochází přes samostatné feature větve a Pull Requesty.
4. **Jazyk implementace:** Veškerý zdrojový kód, identifikátory a komentáře v `src/` jsou výhradně v technické angličtině. Projektová dokumentace a komunikace v repozitáři probíhá v češtině.
5. **Průběžná verifikace:** Funkčnost je soustavně ověřována pomocí testovací sady v `Makefile`.
6. **Časový limit obhajoby:** Struktura prezentace je cílena na oficiální 5minutový časový limit.
7. **Přezkum rozhraní:** Klíčová rozhraní modulů (AST, tabulka symbolů, mezikód) podléhají týmové dohodě před samotnou implementací.

---

## Správa repozitáře a pracovní postup

Práce na projektu je řízena pomocí GitHub Issues a Projects:

* **Milestones:** Každá fáze překladače (lexikální, syntaktická, sémantická analýza, generování kódu) má stanovený milník s interním termínem dokončení.
* **Větve:** Vývoj probíhá v dedikovaných větvích (například: `feat/*`, `fix/*`, `refactor/*`).
* **Pull Requesty:** Každý PR musí být provázán s příslušnou Issue, splňovat formální náležitosti a projít schválením před sloučením do `main`.

---

## Nástroje a automatizace

### Statická analýza a formátování
Lokální kontrola a sjednocení formátování kódu:

```shell
# Kontrola a automatické přeformátování zdrojových kódů
make format

# Kontrola komentářů a stylu
make linter

# Linter s automatickou opravou drobných nedostatků
python3 scripts/lint_comments.py --fix
```

# Instalace pre-commit hooků
```shell
sh ./install_hooks.sh
```

# Generátor hlaviček
```shell
# Přes make
make header-generate 

# Přímo skriptem
python3 scripts/generate_headers.py
```

# Spouštění testů
```shell
# Testování jednotlivých komponent během vývoje
make test-scanner
make test-parser
make test-symtable
make test-codegen

# Spuštění kompletní testovací sady
make test-all
```

# Průběh obhajoby

Obhajoba projektu se skládá ze dvou částí:

    1. Prezentace (~5 minut): Shrnutí architektury překladače, datových toků a organizace týmové práce představené vybraným členem týmu.

    2. Odborná rozprava (~15 minut): Technické dotazy hodnotitelů k implementaci, složitostem použitých struktur a teoretickým základům.


> [!note]
> Textace v pomocných souborech je záměrně psaná česky pro snazší orientaci členů týmu v tom, co přesně je jeho výstupem - aby nedošlo ke "ztrátě v překladu" ve věcech, kde je to zcela nežádoucí a jakékoliv hlubší řešení významu neefektivní

> [!important]
> README v jednotlivých adresářích jsou určena primárně jako interní dokumentace sloužící k zjednonznačnění a snadnějšímu pochopení jednotlivých částí. Pro zajištění jejich maxmální přesnosti a jednoznačnosti **byly** použity nástroje umělé inteligence (Gemini 3.8 flash)