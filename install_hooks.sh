#!/usr/bin/env bash
# scripts/install_hooks.sh
# Aktivace git hooků a sterilizace hlaviček pro tým IFJ26



set -e

# ANSI barvy
BOLD='\033[1m'
DIM='\033[2m'
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
NC='\033[0m'

ROOT_DIR="$(git rev-parse --show-toplevel 2>/dev/null)"
if [ -z "$ROOT_DIR" ]; then
    echo -e "${RED}${BOLD}[CHYBA]${NC} Tento skript musí být spuštěn uvnitř Git repozitáře!"
    exit 1
fi

cd "$ROOT_DIR"

echo -e "${MAGENTA}${BOLD}====================================================${NC}"
echo -e "${CYAN}${BOLD}       IFJ26 :: INSTALACE PROJEKTOVÉ INKVIZICE       ${NC}"
echo -e "${MAGENTA}${BOLD}====================================================${NC}"

# 1. Příprava složky a zápis opraveného pre-commit hooku
echo -e "\n${BLUE}${BOLD}[1/4]${NC} ${DIM}Vytvářím scripts/hooks/pre-commit...${NC}"
mkdir -p scripts/hooks

cat << 'EOF' > scripts/hooks/pre-commit
#!/usr/bin/env bash
# IFJ26 Inkviziční pre-commit hook

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

ROOT_DIR="$(git rev-parse --show-toplevel)"
cd "$ROOT_DIR"

# Filtrovat pouze změněné/přidané C/H soubory v src/
STAGED_FILES=$(git diff --cached --name-only --diff-filter=ACM | grep -E '^src/.*\.(c|h)$' || true)

if [ -z "$STAGED_FILES" ]; then
    exit 0
fi

echo -e "${YELLOW}[HOOK] Spouštím inkvizici nad staged soubory...${NC}"

# 1. Formátování pouze nad staged soubory
if command -v clang-format >/dev/null 2>&1; then
    echo -e "${CYAN}[HOOK] Aplikuji clang-format...${NC}"
    echo "$STAGED_FILES" | xargs clang-format -i
fi

# 2. Generování / úprava hlaviček
if [ -f "scripts/generate_headers.py" ]; then
    echo -e "${CYAN}[HOOK] Aktualizuji hlavičky souborů...${NC}"
    python3 scripts/generate_headers.py $STAGED_FILES
fi

# 3. Znovu přidat automaticky zformátované soubory do stagingu
echo "$STAGED_FILES" | xargs git add

# 4. Validace komentářů a Doxygenu
if [ -f "scripts/lint_comments.py" ]; then
    echo -e "${CYAN}[HOOK] Kontroluji komentáře a dokumentaci...${NC}"
    python3 scripts/lint_comments.py $STAGED_FILES
fi

echo -e "${GREEN}[OK] Všechny soubory prošly inkvizicí.${NC}"
exit 0
EOF

chmod +x scripts/hooks/pre-commit
echo -e "  ${GREEN}✔${NC} Hook vygenerován a nastaven na ${BOLD}chmod +x${NC}"

# 2. Nastavení spustitelných práv pro pomocné python skripty
echo -e "\n${BLUE}${BOLD}[2/4]${NC} ${DIM}Nastavuji práva pro lintery...${NC}"
chmod +x scripts/lint_comments.py 2>/dev/null || true
chmod +x scripts/generate_headers.py 2>/dev/null || true
echo -e "  ${GREEN}✔${NC} Práva skriptů v ${CYAN}scripts/${NC} nastavena"

# 3. Konfigurace Gitu
echo -e "\n${BLUE}${BOLD}[3/4]${NC} ${DIM}Konfiguruji Git core.hooksPath -> scripts/hooks...${NC}"
git config core.hooksPath scripts/hooks
echo -e "  ${GREEN}✔${NC} Git config core.hooksPath: ${BOLD}scripts/hooks${NC}"

# Pojistka: symlink pro starší git verze
HOOK_DEST=".git/hooks/pre-commit"
if [ -d ".git/hooks" ]; then
    rm -f "$HOOK_DEST"
    ln -s "../../scripts/hooks/pre-commit" "$HOOK_DEST"
    echo -e "  ${GREEN}✔${NC} Fallback symlink ${CYAN}.git/hooks/pre-commit${NC} vytvořen"
fi

# 4. Prohnání existujících zdrojáků přes generátor hlaviček
echo -e "\n${BLUE}${BOLD}[4/4]${NC} ${DIM}Generuji chybějící Doxygen hlavičky v src/...${NC}"
if [ -f "scripts/generate_headers.py" ]; then
    python3 scripts/generate_headers.py
else
    echo -e "  ${YELLOW}⚠${NC} ${DIM}scripts/generate_headers.py nenalezen, přeskočeno.${NC}"
fi

# Závěrečné echo
echo -e "\n${MAGENTA}${BOLD}----------------------------------------------------${NC}"
echo -e "${GREEN}${BOLD}      [VÍTĚZSTVÍ] HOOKY BYLY ÚSPĚŠNĚ AKTIVOVÁNY!    ${NC}"
echo -e "${MAGENTA}${BOLD}----------------------------------------------------${NC}"
echo -e "Od této chvíle žádný nekvalitní kód neprojde přes ${CYAN}git commit${NC}.\n"