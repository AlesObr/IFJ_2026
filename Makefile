CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g -Isrc

# Destination for the compiled test binary
scanner_test: src/lexer/lexer.c src/_utils/token.c test/lexer_test/lexer_test.c
	@mkdir -p output
	$(CC) $(CFLAGS) $^ -o output/lexer_test

# Command to run the scanner tests
test-scanner: scanner_test
	@python3 test/test_run.py

# Verify memory leaks using Valgrind
test-scanner-valgrind: scanner_test
	@python3 test/lexer_test/generate_testcases.py
	@python3 test/test_run.py --valgrind

.PHONY: test-scanner test-scanner-valgrind