# AST
**AST neboli abstraktní syntaktický strom** představuje hierarchickou stromovou reprezentaci syntaktické struktury zdrojového programu po očištění od redundantního syntaktického balastu (závorky, oddělovače). Slouží jako přechodová struktura mezi syntaktickou analýzou a sémantickou analýzou / generováním kódu


## Životní cyklus uzlu
- Strom se skládá z tzv `ASTNode`(uzel syntaktického stromu), který nese typ konstrukce (`ASTNodeType`), pozici konstrukce ve zdrojovém textu (jednodušší určování místa, kde se co pokazilo) a unii specifických dat uzlu (literály, operátory, bloky příkazů).
- Alokace a invarianty: Konstruktory uzlů vrací ukazatel na nově alokovaný uzel. Pokud alokace selže (vrací NULL), dochází k systémovému selhání alokátoru, na což navazuje aktivace HErBA protokolu (viz docs/HErBA.md) — okamžité ukončení překladu s interním chybovým kódem 99 bez úniku paměti.
- Destrukce: Strom je deterministicky uvolňován rekurzivním voláním ast_destroy(), které korektně dealokuje veškeré dynamické řetězce (dstring_t), seznamy potomků (ASTNodeList) i uzly samotné.

