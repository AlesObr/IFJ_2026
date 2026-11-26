Využití AI
Kapitoly níže se budou zabývat tím, jaké nástroje a jak byly použity při řešení tohoto
projektu.
• Modely Gemini Flash 3.8, 3.1 Pro, Hydrogen, Argon
• Modely Claude Sonnet 5.5 a Opus 5.5 (Claude Code)
• Github Copilot
• Gitkraken
Moduly
4.1 Úkoly ke kterým byly nástroje GAI1 použity
• Konzultace: Konzultace zadání, rešerše projektu, studijních materiálů, konzultace
architektonického návrhu a technických možností implementace
• CI/CD infrastruktura: Asistence při tvorbě pomocných skriptů a boilerplate kódu
(linter.py, flatten.py, Makefile).
• Testování: Asistance při tvorbě testů (fuzzery pro generování testovacích sad, test
runnery, harnesy)
• Formální verifikace gramatik: Pomoc s tvorbou formálních verifikátorů, které byly
použity pro ověření správnosti a jednotnosti gramatik, které byly týmem vytvořeny.
(Kontrola výpočtu FIRST/FOLLOW/EMPTY množin a ověření bezkonfliktnosti LL1
gramatiky), ověření shody návrhu precedenční tabulky s implementací.
• Ladicí podpora (code review): Asistence při identifikaci logických chyb ve větvících
podmínkách a kontrole toku řízení programu.
• Vizuál dokumentace: Sjednocení formátování a vizuálního stylu diagramů (DKA,
architektonická schémata) do jednotného designového jazyka.
4.2 Typický pracovní postup s GAI
1. Předání kontextu: Předložení problému a dotazu k němu, příslušné části specifikace
jazyka nebo definovaný problémový útržek kódu
1. Iterativní diskuze nad navrhovaným řešení vedoucí k jeho zpřesnění, doladění
2. Kritické posouzení výstupu autorem daného modulu, jeho úprava na míru
architektuře překladače a kontrola souladu se zadáním.
1. Automatizované ověření: Začlenění změn do repozitáře až po úspěšném ověření
překladu bez varování a průchodu sadou automatizovaných testů.
4.3 Pracovní postup u podpůrných skriptů a programů pro vývoj
1. Definice rozhraní a požadavků: Specifikace vstupních a výstupních formátů,
požadovaných kontrol nebo gramatických pravidel pro generování testovacích sad (bez
vazby na interní logiku překladače).
1. Generování a iterativní ladění nástroje: Vygenerování kostry skriptu (linter.py, flatten.
py, test runnery, fuzzery) a její rychlé iterativní doladění nad reálnými testovacími daty a
referenčním interpretem.
1. Validace výstupů: Kontrola, zda vygenerované testovací sady a pomocné skripty
spolehlivě zachycují okrajové stavy a odpovídají formálním požadavkům zadání.