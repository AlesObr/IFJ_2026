CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra  -pedantic -g
INCLUDES = -Iinclude -Isrc
# Kolikrat spustit parser testy (napr. make test-parser REPEAT=5); pri REPEAT>1 jen souhrn
REPEAT ?= 1
# Limit (s) na test dostupnosti Dockeru před spuštěním is_it_ok.sh
DOCKER_PROBE_TIMEOUT ?= 10
# Generování parser testů (make gen-tests): počty, seed náhody a štítek v názvu testů
GEN_RANDOM  ?= 300
GEN_VALID   ?= 1200
GEN_INVALID ?= 3000
GEN_SEED    ?= 2026
GEN_TAG     ?= gen

# ----------------------------------------------------------------------
# Konfigurace odevzdání (parsování loginu z team.json nebo fallback)
# ----------------------------------------------------------------------
LOGIN ?= $(shell python3 -c "import json; t = json.load(open('team.json')); print(t.get('leader') or t['members'][0]['login'])" 2>/dev/null || echo "xobrale00")
ZIP_NAME = $(LOGIN).zip

# ----------------------------------------------------------------------
# Adresáře pro artefakty a výstupy
# ----------------------------------------------------------------------
BUILD_DIR  = build
OUTPUT_DIR = output
DIST_DIR   = $(BUILD_DIR)/submission

# ----------------------------------------------------------------------
# Zdrojové soubory modulů a testů
# ----------------------------------------------------------------------
DSTRING_SRC      = src/_utils/dstring.c
DSTRING_TEST_SRC = test/dstring_test.c

AST_SRC          = src/parser/ast.c $(DSTRING_SRC)
AST_TEST_SRC     = test/ast_test.c

SYMTABLE_SRC      = src/symtable.c $(AST_SRC)
SYMTABLE_TEST_SRC = test/symtable_test.c

LEXER_SRC        = src/lexer/lexer.c src/_utils/token.c $(DSTRING_SRC)
# Doplněn src/_utils/stack.c pro obsluhu odsazení v lexeru
LEXER_SRC        = src/lexer/lexer.c src/_utils/token.c src/_utils/stack.c $(DSTRING_SRC)
LEXER_TEST_SRC   = test/lexer_test/lexer_test.c

# Parser moduly
PRECEDENCE_SRC   = src/parser/precedence.c
PARSER_SRC       = src/parser/parser.c $(PRECEDENCE_SRC) $(AST_SRC) $(LEXER_SRC)
PARSER_TEST_SRC  = test/syntax_test/test_parser.c

SEMANTIC_SRC      = $(wildcard src/semantic_analyzer/*.c) src/symtable.c $(PARSER_SRC)
SEMANTIC_TEST_SRC = test/semantic_test/test_semantic.c

# ----------------------------------------------------------------------
# Cílové binárky (vše konzistentně do BUILD_DIR)
# ----------------------------------------------------------------------
TARGET_DSTRING_ASAN      = $(BUILD_DIR)/test_dstring_asan
TARGET_DSTRING_VALGRIND  = $(BUILD_DIR)/test_dstring_valgrind

TARGET_AST_ASAN          = $(BUILD_DIR)/test_ast_asan
TARGET_AST_VALGRIND      = $(BUILD_DIR)/test_ast_valgrind

TARGET_SYMTABLE_ASAN     = $(BUILD_DIR)/test_symtable_asan
TARGET_SYMTABLE_VALGRIND = $(BUILD_DIR)/test_symtable_valgrind

TARGET_LEXER_TEST        = $(BUILD_DIR)/lexer_test
TARGET_PARSER_TEST       = $(BUILD_DIR)/parser_test
TARGET_SEMANTIC_TEST     = $(BUILD_DIR)/semantic_test

# ----------------------------------------------------------------------
# Phony cíle
# ----------------------------------------------------------------------
.PHONY: all check clean \
        dstring-asan dstring-valgrind dstring-check \
        ast-asan ast-valgrind ast-check test_ast \
        symtable-asan symtable-valgrind symtable-check test_symtable \
        scanner-build test-scanner test-scanner-valgrind \
        parser-build test-parser test-parser-valgrind gen-tests \
        semantic-build test-semantic test-semantic-valgrind gen-sem-tests \
        linter linter-full linter-fix headers format \
        zip make_us_happy dist-prep is-it-ok

# Výchozí cíl: spustí rychlé jednotkové i parser testy
all: check test-parser

check: dstring-check ast-check symtable-check
	@echo "\033[32m✓ Všechny základní subsystémové testy (dstring + AST + symtable) prošly.\033[0m"

# ----------------------------------------------------------------------
# Vytvoření adresářů
# ----------------------------------------------------------------------
$(BUILD_DIR) $(OUTPUT_DIR) $(DIST_DIR):
	@mkdir -p $@

# ----------------------------------------------------------------------
# Unit testy: dstring (ASan & Valgrind)
# ----------------------------------------------------------------------
$(TARGET_DSTRING_ASAN): $(DSTRING_SRC) $(DSTRING_TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -fsanitize=address,undefined $(INCLUDES) $^ -o $@

$(TARGET_DSTRING_VALGRIND): $(DSTRING_SRC) $(DSTRING_TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

dstring-asan: $(TARGET_DSTRING_ASAN)
	@echo "=== [dstring] Běh pod AddressSanitizer & UBSan ==="
	@./$(TARGET_DSTRING_ASAN)

dstring-valgrind: $(TARGET_DSTRING_VALGRIND)
	@echo "=== [dstring] Běh pod Valgrind ==="
	@valgrind --leak-check=full \
	          --show-leak-kinds=all \
	          --track-origins=yes \
	          --error-exitcode=42 \
	          ./$(TARGET_DSTRING_VALGRIND)

dstring-check: dstring-asan dstring-valgrind
	@echo "\033[32m✓ Unit testy dstring prošly.\033[0m"

# ----------------------------------------------------------------------
# Unit testy: AST (ASan & Valgrind)
# ----------------------------------------------------------------------
$(TARGET_AST_ASAN): $(AST_SRC) $(AST_TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -fsanitize=address,undefined $(INCLUDES) $^ -o $@

$(TARGET_AST_VALGRIND): $(AST_SRC) $(AST_TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

ast-asan: $(TARGET_AST_ASAN)
	@echo "=== [AST] Běh pod AddressSanitizer & UBSan ==="
	@ASAN_OPTIONS=detect_leaks=0 ./$(TARGET_AST_ASAN)

ast-valgrind: $(TARGET_AST_VALGRIND)
	@echo "=== [AST] Běh pod Valgrind ==="
	@valgrind --leak-check=full \
	          --show-leak-kinds=all \
	          --track-origins=yes \
	          --error-exitcode=42 \
	          ./$(TARGET_AST_VALGRIND)

ast-check: ast-asan ast-valgrind
	@echo "\033[32m✓ Unit testy AST prošly.\033[0m"

test_ast: ast-asan

# ----------------------------------------------------------------------
# Unit testy: symtable (ASan & Valgrind)
# ----------------------------------------------------------------------
$(TARGET_SYMTABLE_ASAN): $(SYMTABLE_SRC) $(SYMTABLE_TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -fsanitize=address,undefined $(INCLUDES) $^ -o $@

$(TARGET_SYMTABLE_VALGRIND): $(SYMTABLE_SRC) $(SYMTABLE_TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

symtable-asan: $(TARGET_SYMTABLE_ASAN)
	@echo "=== [symtable] Běh pod AddressSanitizer & UBSan ==="
	@./$(TARGET_SYMTABLE_ASAN)

symtable-valgrind: $(TARGET_SYMTABLE_VALGRIND)
	@echo "=== [symtable] Běh pod Valgrind ==="
	@valgrind --leak-check=full \
	          --show-leak-kinds=all \
	          --track-origins=yes \
	          --error-exitcode=42 \
	          ./$(TARGET_SYMTABLE_VALGRIND)

symtable-check: symtable-asan symtable-valgrind
	@echo "\033[32m✓ Unit testy symtable prošly.\033[0m"

test_symtable: symtable-asan

# ----------------------------------------------------------------------
# Lexer / Scanner testy
# ----------------------------------------------------------------------
$(TARGET_LEXER_TEST): $(LEXER_SRC) $(LEXER_TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

scanner-build: $(TARGET_LEXER_TEST)

test-scanner-basic: scanner-build
	@python3 test/test_run.py --suit basic --timeout 0.5

test-scanner-advanced: scanner-build
	@python3 test/test_run.py --suit advanced --timeout 0.5

test-scanner: scanner-build
	@python3 test/test_run.py --timeout 0.5

test-scanner-valgrind: scanner-build
	@python3 test/test_run.py --valgrind --suit advanced --timeout 0.5

# ----------------------------------------------------------------------
# Parser & AST testy
# ----------------------------------------------------------------------
$(TARGET_PARSER_TEST): $(PARSER_SRC) $(PARSER_TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

parser-build: $(TARGET_PARSER_TEST)

test-parser: parser-build
	@echo "=== Spuštění parser testů (rychlý běh) ==="
	@python3 test/syntax_test/test_runner.py --bin ./$(TARGET_PARSER_TEST) --dir test/generated_tests --check-ast --repeat $(REPEAT)

test-parser-valgrind: parser-build
	@echo "=== Spuštění parser testů pod Valgrindem ==="
	@python3 test/syntax_test/test_runner.py --bin ./$(TARGET_PARSER_TEST) --dir test/generated_tests --check-ast --valgrind --repeat $(REPEAT)

# Náhodné programy do seed/random, z nich a ze základních seedů mutace do test/generated_tests.
# Oracle (parser Scaly) smaže rozbité validní i nepotvrzené chybové testy a vytvoří .ast.
# Testy se stejným GEN_TAG se nejdřív smažou, ostatní v adresáři zůstanou.
gen-tests:
	@find test/generated_tests -name 'test_*_$(GEN_TAG)_*' -delete
	@cd test/syntax_test && \
	python3 program_generator.py --out-dir seed/random --count $(GEN_RANDOM) --seed $(GEN_SEED) --stress && \
	python3 fuzzer.py --seeds-dir seed/basic seed/random --out-dir ../generated_tests \
		--valid-count $(GEN_VALID) --invalid-count $(GEN_INVALID) --seed $(GEN_SEED) \
		--tag $(GEN_TAG) --copy-seeds --oracle

# ----------------------------------------------------------------------
# Testy sémantické analýzy (parser + sémantika, očekávaný kód je v názvu testu)
# ----------------------------------------------------------------------
# cases/: ruční testy pravidel zadání (make_cases.py), generated/: validní seedy a jejich
# mutace s jednou sémantickou chybou (make gen-sem-tests). Jen vybrané: make test-semantic SEM_FILTER=e6_
SEM_TEST_DIRS ?= test/semantic_test/cases test/semantic_test/generated
SEM_FILTER    ?=
SEM_PER_SEED  ?= 3

$(TARGET_SEMANTIC_TEST): $(SEMANTIC_SRC) $(SEMANTIC_TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

semantic-build: $(TARGET_SEMANTIC_TEST)

test-semantic: semantic-build
	@echo "=== Spuštění testů sémantiky ==="
	@python3 test/semantic_test/sem_runner.py --bin ./$(TARGET_SEMANTIC_TEST) --dir $(SEM_TEST_DIRS) \
		$(if $(SEM_FILTER),--filter $(SEM_FILTER))

test-semantic-valgrind: semantic-build
	@echo "=== Spuštění testů sémantiky pod Valgrindem ==="
	@python3 test/semantic_test/sem_runner.py --bin ./$(TARGET_SEMANTIC_TEST) --dir $(SEM_TEST_DIRS) \
		--valgrind $(if $(SEM_FILTER),--filter $(SEM_FILTER))

# Ruční testy se přegenerují z make_cases.py, mutace z validních seedů (přeskočí ty, které
# neprojdou parserem); obsah obou adresářů se přepíše
gen-sem-tests: parser-build
	@python3 test/semantic_test/make_cases.py
	@python3 test/semantic_test/sem_mutator.py \
		--seeds-dir test/syntax_test/seed/basic test/syntax_test/seed/random \
		--out-dir test/semantic_test/generated --per-seed $(SEM_PER_SEED) --seed $(GEN_SEED) \
		--parser-bin ./$(TARGET_PARSER_TEST)

# ----------------------------------------------------------------------
# Formátování a Lintery
# ----------------------------------------------------------------------
linter:
	@python3 scripts/lint_comments.py

linter-full:
	@python3 scripts/generate_headers.py
	@python3 scripts/lint_comments.py

linter-clear:
	@python3 scripts/generate_headers.py
	@python3 scripts/lint_comments.py --strip-dev --fix
	@python3 scripts/lint_comments.py

linter-fix:
	@python3 scripts/lint_comments.py --fix

headers:
	@python3 scripts/generate_headers.py

format:
	@clang-format -i src/*.c src/*.h src/**/*.c src/**/*.h 2>/dev/null || true
	@echo "✓ Kód zformátován podle .clang-format"

# ----------------------------------------------------------------------
# Zabalení archivu pro odevzdání (make zip / make make_us_happy)
# ----------------------------------------------------------------------
dist-prep: format linter-full | $(DIST_DIR)
	@rm -rf $(DIST_DIR)/*
	@if [ -f scripts/flatten.py ]; then \
		echo "=== Zplošťuji projekt pomocí flatten.py ==="; \
		python3 scripts/flatten.py --out $(DIST_DIR) --no-archive || exit 1; \
	else \
		echo "=== Kopíruji zdrojové soubory do $(DIST_DIR) ==="; \
		cp -r src/* $(DIST_DIR)/; \
		[ -f Makefile ] && cp Makefile $(DIST_DIR)/; \
	fi

zip: dist-prep
	@echo "=== Vytvářím odevzdávací archiv $(ZIP_NAME) ==="
	@rm -f $(ZIP_NAME)
	@cd $(DIST_DIR) && zip -r ../../$(ZIP_NAME) . -x "*.git*" "*build*" "*output*" "*.vscode*"
	@echo "\033[32m✓ Archiv $(ZIP_NAME) byl úspěšně vygenerován.\033[0m"
	@echo "--- Obsah archivu: ---"
	@unzip -l $(ZIP_NAME)
	@$(MAKE) --no-print-directory is-it-ok

make_us_happy: zip

# Oficiální kontrola archivu; log ifj26_is-it-ok_*.log zůstává v $(BUILD_DIR)/
is-it-ok:
	@if [ ! -x ./is_it_ok.sh ]; then \
		echo "\033[31m✗ Chybí spustitelný ./is_it_ok.sh, archiv nelze zkontrolovat.\033[0m"; exit 1; \
	fi
	@echo "=== Kontrola archivu $(ZIP_NAME) pomocí is_it_ok.sh ==="
	@mkdir -p $(BUILD_DIR)
	@# is_it_ok.sh volá `docker info` bez časového limitu; když Docker (Desktop) neběží,
	@# visí navždy. Neodpovídající Docker proto skryjeme a kontrola proběhne bez kontejneru.
	@if command -v docker >/dev/null 2>&1 && ! timeout $(DOCKER_PROBE_TIMEOUT) docker info >/dev/null 2>&1; then \
		echo "\033[33m⚠ Docker neodpověděl do $(DOCKER_PROBE_TIMEOUT) s (běží Docker Desktop?), kontrola proběhne bez překladu v kontejneru.\033[0m"; \
		cd $(BUILD_DIR) && DOCKER_HOST=unix:///nonexistent-ifj26.sock ../is_it_ok.sh ../$(ZIP_NAME); \
	else \
		cd $(BUILD_DIR) && ../is_it_ok.sh ../$(ZIP_NAME); \
	fi

# ----------------------------------------------------------------------
# Úklid
# ----------------------------------------------------------------------
clean:
	rm -rf $(BUILD_DIR) $(OUTPUT_DIR) $(ZIP_NAME)