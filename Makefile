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

CODEGEN_SRC       = $(wildcard src/code_gen/*.c) $(SEMANTIC_SRC)
CODEGEN_TEST_SRC  = test/codegen_test/test_codegen.c

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
TARGET_CODEGEN_TEST      = $(BUILD_DIR)/codegen_test
TARGET_COMPILER          = $(BUILD_DIR)/ifj26
COMPILER_SRC             = src/main.c $(CODEGEN_SRC)

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
        codegen-build test-codegen test-codegen-valgrind gen-codegen-tests \
        compiler test-full ext-status dka tables \
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
# Testy generátoru kódu (parser + sémantika + codegen -> interpret -> porovnání s výstupem Scaly)
# ----------------------------------------------------------------------
# programs/: ruční testy (make_programs.py), random/: náhodné programy bez běhových chyb,
# random_simple/: náhodné programy bez if, while a return, k tomu základní seedy.
# Interpret: INTERP=cesta (např. INTERP=test/codegen_test/ic26int.sh), jinak ic26int z PATH,
# jinak náhradní test/codegen_test/ifjcode_interp.py. Vygenerovaný kód a výstupy: CG_KEEP=adresář.
# Testy s nepodporovanými konstrukcemi se přeskočí přes CG_EXCLUDE, např.
#   make test-codegen CG_EXCLUDE=if,while,for,return
# (seznam konstrukcí: python3 test/codegen_test/codegen_runner.py --help)
CG_TEST_DIRS ?= test/codegen_test/programs test/codegen_test/random test/codegen_test/random_simple \
                test/syntax_test/seed/basic
CG_FILTER    ?=
CG_EXCLUDE   ?=
CG_KEEP      ?=
CG_RANDOM    ?= 120
INTERP       ?=
CG_RUNNER     = python3 test/codegen_test/codegen_runner.py --bin ./$(TARGET_CODEGEN_TEST) \
	--dir $(CG_TEST_DIRS) $(if $(INTERP),--interp $(INTERP)) $(if $(CG_FILTER),--filter $(CG_FILTER)) \
	$(if $(CG_EXCLUDE),--exclude $(CG_EXCLUDE)) $(if $(CG_KEEP),--keep $(CG_KEEP))

$(TARGET_CODEGEN_TEST): $(CODEGEN_SRC) $(CODEGEN_TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

codegen-build: $(TARGET_CODEGEN_TEST)

test-codegen: codegen-build
	@echo "=== Spuštění testů generátoru kódu ==="
	@$(CG_RUNNER)

test-codegen-valgrind: codegen-build
	@echo "=== Spuštění testů generátoru kódu (překladač pod Valgrindem) ==="
	@$(CG_RUNNER) --valgrind

# Pomalé (každý program se jednou spustí Scalou, ~1-2 s): ruční testy a náhodné programy
# včetně očekávaných výstupů. Náhodné programy, které ve Scale selžou, přetečou Int nebo
# neprojdou parserem, se vyřadí.
gen-codegen-tests: parser-build
	@cd test/codegen_test && python3 make_programs.py && \
	python3 scala_expected.py --dir programs --force && \
	rm -rf random && \
	python3 ../syntax_test/program_generator.py --out-dir random --count $(CG_RANDOM) \
		--seed $(GEN_SEED) --runtime-safe && \
	python3 scala_expected.py --dir random --filter-random --parser-bin ../../$(TARGET_PARSER_TEST) && \
	rm -rf random_simple && \
	python3 ../syntax_test/program_generator.py --out-dir random_simple --count $(CG_RANDOM) \
		--seed $(GEN_SEED) --runtime-safe --no-control && \
	python3 scala_expected.py --dir random_simple --filter-random --parser-bin ../../$(TARGET_PARSER_TEST)

# ----------------------------------------------------------------------
# Celý překladač (src/main.c) a testy celé pipeline nad ním
# ----------------------------------------------------------------------
$(TARGET_COMPILER): $(COMPILER_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

compiler: $(TARGET_COMPILER)

# Sémantické testy (očekávaný kód; ok_* tedy musí projít i generováním kódu) a end-to-end
# testy generátoru, obojí se skutečným překladačem build/ifj26
test-full: compiler
	@echo "=== Celý překladač: kódy chyb (testy sémantiky) ==="
	-@python3 test/semantic_test/sem_runner.py --bin ./$(TARGET_COMPILER) --dir $(SEM_TEST_DIRS)
	@echo "=== Celý překladač: kódy chyb (testy parseru; validní musí projít celou pipeline) ==="
	-@python3 test/syntax_test/test_runner.py --bin ./$(TARGET_COMPILER) --dir test/generated_tests \
		--quiet
	@echo "=== Celý překladač: výstup validních testů parseru (s neprázdným .out) ==="
	-@python3 test/codegen_test/codegen_runner.py --bin ./$(TARGET_COMPILER) --dir test/generated_tests \
		--skip-empty-out --group-by-seed $(if $(INTERP),--interp $(INTERP))
	@echo "=== Celý překladač: výstup programů (testy generátoru) ==="
	@python3 test/codegen_test/codegen_runner.py --bin ./$(TARGET_COMPILER) --dir $(CG_TEST_DIRS) \
		$(if $(INTERP),--interp $(INTERP)) $(if $(CG_EXCLUDE),--exclude $(CG_EXCLUDE))

# Stav rozšíření po fázích (parser -> sémantika -> generátor -> běh -> výstup) nad cílenými
# programy v test/codegen_test/extensions (make_ext_programs.py, výstupy: scala_expected.py)
ext-status: parser-build semantic-build compiler
	@python3 test/codegen_test/ext_status.py $(if $(INTERP),--interp $(INTERP))

# ----------------------------------------------------------------------
# DKA lexeru: kontrola proti skutečnému lexeru a export diagramů do docs/dka
# ----------------------------------------------------------------------
# Precedenční tabulka (ze zdrojáku parseru, ověřená na parser_test) a LL tabulka z gramatiky.
#   make tables                       jen precedence
#   make tables GRAMMAR=cesta/ll.txt  i LL tabulka; výstup v docs/tables (.tex, .md, .csv)
GRAMMAR ?=
tables: parser-build
	@python3 test/formal_verify/gen_tables.py $(if $(GRAMMAR),all $(GRAMMAR),prec)

# DKA_URL = adresa, kam se nahraje docs/dka (index.html); s ní vzniknou i PDF s odkazy na web.
#   make dka DKA_URL=https://example.org/ifj26/dka/
DKA_URL ?=
dka: scanner-build
	@python3 test/formal_verify/lexer_dka.py --base-url "$(DKA_URL)"

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
	@# Bez vylučovacích vzorů: DIST_DIR je vygenerovaný a obsahuje jen soubory k odevzdání
	@# (vzor "*build*" dříve tiše vyřadil i code_gen_buildins.c/.h)
	@cd $(DIST_DIR) && zip -r ../../$(ZIP_NAME) .
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