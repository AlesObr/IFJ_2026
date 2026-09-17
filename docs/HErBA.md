# Co je to HErBA

**HErBA - Hierarchical Error Bubbling Architecture** je systém pro řešení chyb. Zatímco není úplně neobvyklé, že se při jakékoliv chybě program *"zabije"* okamžitě pomocí `exit()`, my to takto dělat nebudeme. Pokud během překladu nastane chyba, bude chyba probublávat za použití ukazatelů na chybový stav. Pokud se volající funkci z volaného nevrátí 0, i tento ukončí veškeré své fungování a bude tuto chybu propagovat.

# Proč ji chceme používat

Protože tím dáme každé části překladače možnost po sobě uklidit paměť

# CaTF

**CaTF - Catastrophic Translator Failure** je systém pro okamžité ukončení překladače. Bude se využívat jen tehdy, kdy:

1. Není možné/bezpečné se pokoušet překladač ukončit pomocí HErBA

2. Není efektivní/reálné spouštět HErBA (zpravidla v situaci, kdy není reálné aktuálně běžící funkci ukončit jednoduchým způsobem a nechat ji doběhnout by stálo příliš mnoho práce). 

> [!Danger]
> Použití CaTF je pouze krajní řešení, které se smí použít jen tehdy, kdy se na tom shodne vedoucí a aspoň jeden další člen týmu (vyjma přápadu kdy selže `malloc()`, `realloc` nebo `free()` ). Použití CaTF musí být vždy dokumentováno jeho pachatelem, alespoň v dokumentačním komentáři u funkce a přímo na místě jeho použití

# Použití
