CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra  -pedantic -g
INCLUDES = -Iinclude -Isrc

# ----------------------------------------------------------------------
# Konfigurace odevzdání (parsování loginu z team.json nebo fallback)
# ----------------------------------------------------------------------
LOGIN ?= $(shell python3 -c "import json; print(json.load(open('team.json'))['leader'])" 2>/dev/null || echo "xobrale00")
ZIP_NAME = $(LOGIN).zip

# ----------------------------------------------------------------------
# Adresáře pro artefakty a výstupy
# ----------------------------------------------------------------------
BUILD_DIR  = build
OUTPUT_DIR = output
DIST_DIR   = $(BUILD_DIR)/dist

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

# ----------------------------------------------------------------------
# Phony cíle
# ----------------------------------------------------------------------
.PHONY: all check clean \
        dstring-asan dstring-valgrind dstring-check \
        ast-asan ast-valgrind ast-check test_ast \
        symtable-asan symtable-valgrind symtable-check test_symtable \
        scanner-build test-scanner test-scanner-valgrind \
        parser-build test-parser test-parser-valgrind \
        linter linter-full linter-fix headers format \
        zip make_us_happy dist-prep

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
	@python3 test/syntax_test/test_runner.py --bin ./$(TARGET_PARSER_TEST) --dir test/generated_tests --check-ast

test-parser-valgrind: parser-build
	@echo "=== Spuštění parser testů pod Valgrindem ==="
	@python3 test/syntax_test/test_runner.py --bin ./$(TARGET_PARSER_TEST) --dir test/generated_tests --check-ast --valgrind

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
		python3 scripts/flatten.py --out $(DIST_DIR); \
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

make_us_happy: zip

# ----------------------------------------------------------------------
# Úklid
# ----------------------------------------------------------------------
clean:
	rm -rf $(BUILD_DIR) $(OUTPUT_DIR) $(ZIP_NAME)