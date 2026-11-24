# 4.  Využití AI
Kapitoly níže se budou zabývat tím, jaké nástroje a jak byly použity při řešení tohoto 
projektu.
•  Modely Gemini Flash 3.8, 3.1 Pro, Hydrogen, Argon
•  Modely Claude Sonnet 5.5 a Opus 5.5 (Claude Code)
•  Github Copilot

Moduly
## 4.1  Úkoly ke kterým byly nástroje GUI1 použity
•  Konzultace:  Konzultace  zadání,  rešerše  projektu,  studijních  materiálů,  konzultace 
architektonického návrhu a technických možností implementace
•  CI/CD infrastruktura: Asistence při tvorbě pomocných skriptů a boilerplate kódu 
(linter.py, flatten.py, Makefile).
•  Testování: Asistance při tvorbě testů (fuzzery pro generování testovacích sad, test 
runnery, harnesy)
•  Formální verifikace gramatik: Pomoc s tvorbou formálních verifikátorů, které byly 
použity pro ověření správnosti a jednotnosti gramatik, které byly týmem vytvořeny. 
(Kontrola  výpočtu  FIRST/FOLLOW/EMPTY  množin  a  ověření  bezkonfliktnosti  LL1 
gramatiky), ověření shody návrhu precedenční tabulky s implementací.
•  Ladicí podpora (code review): Asistence při identifikaci logických chyb ve větvících 
podmínkách a kontrole toku řízení programu.
•  Vizuální  dokumentace:  Sjednocení  formátování  a  vizuálního  stylu  diagramů  (DKA, 
architektonická schémata) do jednotného designového jazyka.
## 4.2  Typický pracovní postup s GUI
1.  Předání kontextu: Předložení problému a dotazu k němu, příslušné části specifikace 
jazyka nebo definovaný problémový útržek kódu
2.  Iterativní diskuze nad navrhovaným řešení vedoucí k jeho zpřesnění, doladění
3.  Kritické  posouzení  výstupu  autorem  daného  modulu,  jeho  úprava  na  míru 
architektuře překladače a kontrola souladu se zadáním.
4.  Automatizované  ověření:  Začlenění  změn  do  repozitáře  až  po  úspěšném  ověření 
překladu bez varování a průchodu sadou automatizovaných testů.
## 4.3  Pracovní postup u podpůrných skriptů a programů pro vývoj
1.  Definice  rozhraní  a  požadavků:  Specifikace  vstupních  a  výstupních  formátů, 
požadovaných kontrol nebo gramatických pravidel pro generování testovacích sad (bez 
vazby na interní logiku překladače).
2. Generování a iterativní ladění nástroje: Vygenerování kostry skriptu (linter.py, flatten.
py, test runnery, fuzzery) a její rychlé iterativní doladění nad reálnými testovacími daty a 
referenčním interpretem.
3.  Validace  výstupů:  Kontrola,  zda  vygenerované  testovací  sady  a  pomocné  skripty 
spolehlivě zachycují okrajové stavy a odpovídají formálním požadavkům zadání.
1 GUI = Generativní uměla inteligence

Využití AI
## 4.4  Problémy při používání umělé inteligence
Jazykové  modely  jsou  stále  jenom  stroj,  navíc  nikoliv  deterministický  založený  na 
pravděpodobnostních modelech, který nedisponuje veškerým věděním lidstva, ač má k 
tomuto  stavu  poměrně  blízko  a  nemá  nekonečnou  paměť.  Z  těchto  důvodů  plynulo 
několik zásadních problémů.
1.  Agresivita  in-line  našeptávačů  (Github  Copilot):  Tyto  nástoje  byly  sice  dost  často 
přínosné, pro přesnou formulaci komentářů a drobných změnách v kódu (našeptávání 
názvů  proměnných),  nicméně  poměrně  často  se  tato  asistivní  funkce  zvrhla  do 
podoby, kdy si Copilot začal věřit natolik, že dostal chuť vkládat celé bloky funkcí, 
které ale byly na první pohled nesmyslné. Z tohoto důvodu byla stran této nadměrné 
kreativity  tato  funkce  vypnuta  a  využíváno  spíše  standardních  našeptávačů  typu 
Intellisense.
2.  Ztráta  kontextu:  dlouhodobě  nešlo  vést  diskuzi  v  rámci  jednoho  kontextu,  neboť 
modely postupně "zapomínaly" kontext dříve probíraných témat.
3.  Občasně  příliš  veliký  sokratický  přístup  modelu  při  jisté  konfiguraci  jeho 
konverzační paměti způsoboval nadměrné tendence k vedení dlouhých filozofických 
debat o zadání a také to, že model vyčerpal svůj bezplatný tokenový limit aniž by byl 
jeho výstup ve finále jakkoliv užitečný.
