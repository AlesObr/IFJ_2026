CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic -g
INCLUDES = -Iinclude -Isrc

# Adresáře pro artefakty
BUILD_DIR = build
OUTPUT_DIR = output

# Zdrojáky pro izolovaný dstring test
# (Pokud máte dstring v src/_utils/, upravte cestu na src/_utils/dstring.c)
DSTRING_SRC = src/_utils/dstring.c
DSTRING_TEST_SRC = test/dstring_test.c

TARGET_DSTRING_ASAN = $(BUILD_DIR)/test_dstring_asan
TARGET_DSTRING_VALGRIND = $(BUILD_DIR)/test_dstring_valgrind

# Cíle
.PHONY: all check asan valgrind scanner-test test-scanner test-scanner-valgrind \
        linter linter-full linter-fix headers format clean

# Výchozí target při spuštění pouhého "make"
all: check

# ----------------------------------------------------------------------
# Adresáře
# ----------------------------------------------------------------------
$(BUILD_DIR) $(OUTPUT_DIR):
	@mkdir -p $@

# ----------------------------------------------------------------------
# Unit testy: dstring (ASan & Valgrind)
# ----------------------------------------------------------------------
$(TARGET_DSTRING_ASAN): $(DSTRING_SRC) $(DSTRING_TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -fsanitize=address,undefined $(INCLUDES) $^ -o $@

$(TARGET_DSTRING_VALGRIND): $(DSTRING_SRC) $(DSTRING_TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

dstring-asan: $(TARGET_DSTRING_ASAN)
	@echo "=== [1/2] Běh dstring pod AddressSanitizer & UBSan ==="
	@./$(TARGET_DSTRING_ASAN)

dstring-valgrind: $(TARGET_DSTRING_VALGRIND)
	@echo "=== [2/2] Běh dstring pod Valgrind ==="
	@valgrind --leak-check=full \
	          --show-leak-kinds=all \
	          --track-origins=yes \
	          --error-exitcode=42 \
	          ./$(TARGET_DSTRING_VALGRIND)

dstring-check: dstring-asan dstring-valgrind
	@echo "\033[32m✓ Všechny unit testy dstring prošly.\033[0m"

# ----------------------------------------------------------------------
# Lexer / Scanner testy
# ----------------------------------------------------------------------
$(OUTPUT_DIR)/lexer_test: src/lexer/lexer.c src/_utils/token.c $(DSTRING_SRC) test/lexer_test/lexer_test.c | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

scanner-build: $(OUTPUT_DIR)/lexer_test

test-scanner: scanner-build
	@python3 test/test_run.py

test-scanner-valgrind: scanner-build
	@python3 test/lexer_test/generate_testcases.py
	@python3 test/test_run.py --valgrind

# ----------------------------------------------------------------------
# Formátování a Lintery
# ----------------------------------------------------------------------
linter:
	@python3 scripts/lint_comments.py

linter-full:
	@python3 scripts/generate_headers.py
	@python3 scripts/lint_comments.py

linter-fix:
	@python3 scripts/lint_comments.py --fix

headers:
	@python3 scripts/generate_headers.py

format:
	@clang-format -i src/*.c src/*.h src/**/*.c src/**/*.h 2>/dev/null || true
	@echo "✓ Kód zformátován podle .clang-format"

# ----------------------------------------------------------------------
# Úklid
# ----------------------------------------------------------------------
clean:
	rm -rf $(BUILD_DIR) $(OUTPUT_DIR)