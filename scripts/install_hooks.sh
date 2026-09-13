#!/usr/bin/env bash
# scripts/install_hooks.sh
# Aktivace git hooků pro tým IFJ26

set -e

GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

ROOT_DIR="$(git rev-parse --show-toplevel 2>/dev/null)"
if [ -z "$ROOT_DIR" ]; then
    echo "Chyba: Tento skript musí být spuštěn uvnitř Git repozitáře!"
    exit 1
fi

cd "$ROOT_DIR"

echo -e "${BLUE}[+] Nastavuji práva pro spustitelné skripty...${NC}"
chmod +x scripts/lint_comments.py 2>/dev/null || true
chmod +x scripts/hooks/pre-commit 2>/dev/null || true

echo -e "${BLUE}[+] Konfiguruji Git core.hooksPath -> scripts/hooks...${NC}"
# Moderní přístup: Git bere hooky přímo z verzované složky
git config core.hooksPath scripts/hooks

# Pojistka: Pro starší verze gitu nebo nestandardní klienty vytvoříme i symlink
HOOK_DEST=".git/hooks/pre-commit"
if [ -d ".git/hooks" ]; then
    rm -f "$HOOK_DEST"
    ln -s "../../scripts/hooks/pre-commit" "$HOOK_DEST"
fi

echo -e "${GREEN}[VÍTĚZSTVÍ] Hooky byly úspěšně aktivovány!${NC}"
echo "Od této chvíle žádný nekvalitní kód neprojde přes 'git commit'."