Kapitoly níže se budou zabývat tím, jaké nástroje byly použity a jak byly použity při řešení tohoto projektu.
•	Modely Gemini Flash 3.8, 3.1 Pro, Hydrogen, Argon
•	Modely Claude Sonnet 5.5 a Opus 5.5 (Claude Code)
•	GitHub Copilot
•	GitKraken
4.1	Úkoly, ke kterým byly nástroje GAI použity
•	Konzultace: Konzultace zadání, rešerše projektu, studijních materiálů, konzultace architektonického návrhu a technických možností implementace
•	CI/CD infrastruktura: Asistence při tvorbě pomocných skriptů a boilerplate kódu (lint_comments.py, flatten.py, Makefile).
•	Testování: Asistence při tvorbě testů a testovacích nástrojů (fuzzery pro generování testovacích sad, test runnery, harnesy, seedy)
•	Formální verifikace gramatik: Pomoc s tvorbou formálních verifikátorů gramatik, které tým vytvořil (kontrola výpočtu množin FIRST/FOLLOW/EMPTY a ověření bezkonfliktnosti LL(1) gramatiky), a ověření shody návrhu precedenční tabulky s implementací.
•	Ladicí podpora (code review): Asistence při identifikaci logických chyb ve větvících podmínkách a kontrole toku řízení programu a případná pomoc s jejich opravou.
•	Dokumentace: Sjednocení formátování a vizuálního stylu diagramů (DKA, architektonická schémata) do jednotného designového jazyka. Nakonec bylo těchto výstupů použito pouze pro vygenerování interaktivního DKA (viz kapitola 6) pro web. Dále byla AI využita jako asistující korektor pro ladění zejména jazykové stránky této dokumentace.
•	Pomoc s opravou precedenční analýzy: Při závěrečné kontrole bylo zjištěno, že původní analýza výrazů neodpovídá požadovanému tabulkovému přístupu. Z časových důvodů byla řídicí logika přepisována za asistence jazykového modelu (LLM), který následně asistoval i při reinte	graci s existujícími podfunkcemi. Samotná precedenční tabulka i pomocné funkce jsou autorskou prací týmu a veškerý vygenerovaný kód prošel ruční revizí a testováním členem týmu, který jeho funkcionalitě plně rozumí.
4.2	Typický pracovní postup s GAI
1.	Předání kontextu: Předložení problému a dotazu k němu, příslušné části specifikace jazyka nebo definovaný problémový útržek kódu
2.	Iterativní diskuze nad navrhovaným řešením vedoucí k jeho zpřesnění a doladění
3.	Kritické posouzení výstupu autorem daného modulu, jeho úprava na míru architektuře překladače a kontrola souladu se zadáním.
4.	Ověření pomocí testovací sady: Byla stanovena zásada zanesení změn do repozitáře až po úspěšném ověření překladu, ideálně bez varování, a průchodu sadou automatizovaných testů.
4.3	Pracovní postup u podpůrných skriptů a programů pro vývoj
1. Definice rozhraní a požadavků: Specifikace vstupních a výstupních formátů, požadovaných kontrol nebo gramatických pravidel pro generování testovacích sad (bez vazby na interní logiku překladače).
2. Generování a iterativní ladění nástroje: Vygenerování kostry skriptu (lint_comments.py, flatten.py, test runnery, fuzzery) a její rychlé iterativní doladění nad reálnými testovacími daty a referenčním interpretem.
3. Validace výstupů: Kontrola, zda vygenerované testovací sady a pomocné skripty spolehlivě zachycují okrajové stavy a odpovídají formálním požadavkům zadání.
4.4	Problémy při používání umělé inteligence
Jazykové modely jsou stále jen stroje, navíc nedeterministické a založené na pravděpodobnostních základech. Nedisponují veškerým věděním lidstva, ač k tomu mají poměrně blízko, a nemají ani nekonečnou paměť. Z toho plynulo několik zásadních problémů.
1.	Agresivita in-line našeptávačů (Github Copilot): Tyto nástroje byly sice dost často přínosné pro přesnou formulaci komentářů a při drobných změnách v kódu (našeptávání názvů proměnných), nicméně poměrně často se tato asistivní funkce zvrhla do podoby, kdy si Copilot začal věřit natolik, že dostal chuť vkládat celé bloky funkcí, které ale byly na první pohled nesmyslné. Z těchto důvodů byla tato funkce kvůli nadměrné kreativitě vypnuta a využívaly se spíše standardní našeptávače.
2.	Ztráta kontextu: Dlouhodobě nešlo vést diskuzi v rámci jednoho kontextu, neboť modely postupně "zapomínaly" kontext dříve probíraných témat.
3.	Občasně příliš veliký sokratický přístup modelu při jisté konfiguraci jeho konverzační paměti způsoboval nadměrné tendence k vedení dlouhých filozofických debat o zadání a také to, že model vyčerpal svůj bezplatný tokenový limit, aniž by byl jeho výstup ve finále jakkoliv užitečný.