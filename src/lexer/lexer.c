/**
 * @file lexer.c
 * @brief Lexer module implementation for IFJ26 compiler.
 * @author Aleš Obr (skolp)
 * @note Standard: ISO C11
 */

#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>


typedef enum {
	STATE_START,
} LexerState;

// Dummy implementation of the lexer. This should be replaced with a full implementation.
static FILE *source_file = NULL;
static size_t current_line = 1;

void lexer_init(FILE *input) {
	source_file = input;
	current_line = 1;
}

Token lexer_get_next_token(void) {
	Token token;
	token_init(&token);
	token.line = current_line;

	if (!source_file) {
		token.type = TOKEN_EOF;
		return token;
	}

	LexerState state = STATE_START;

	while (1) {
		int c = fgetc(source_file);

		switch (state) {
			case STATE_START:
				// Dummy
				if (c == ' ' || c == '\t' || c == '\r') {
					continue;
				}

				// Dummy
				if (c == '\n') {
					current_line++;
					token.line = current_line;
					continue;
				}

				// Dummy
				if (c == EOF) {
					token.type = TOKEN_EOF;
					return token;
				}

				// Dummy
				if (c == '+') {
					token.type = TOKEN_PLUS;
					return token;
				}

				// Here add more cases for other token types (identifiers, numbers, strings, etc.) as needed.
				fprintf(stderr, "Lexical error on line %zu: unexpected character '%c' (ASCII %d)\n",
						current_line, (char)c, c);
				exit(1);

			default:
				fprintf(stderr, "Internal compiler error: unknown lexer state\n");
				exit(99);
		}
	}
}
