/**
 * @project IFJ26 Compiler
 * @file lexer.c
 * @brief Lexer module implementation for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @author Jiří Kolek (xkolekj00)
 * @author admiral <Jkolikovec@gmail.com>
 * @note Standard: ISO C11
 */

#include "lexer.h"

#include <ctype.h>

#include "../_utils/dstring.h"
#include "../_utils/error.h"
#include "../_utils/token.h"
#include <stdio.h>
#include <stdlib.h>

#define NEWLINE 10
#define WHITESPACE 32

/*All Global vars. Below are the important ones to keep current state of Lexer */

static FILE *source_file =
	NULL; /** The input source file stream for the lexer. In fact it is STDIN */
static int current_line = 1;
static int current_col = 1;
static int last_line_len = 0;

void lexer_init(FILE *input) {
	source_file = input;
	current_line = 1;
	current_col = 1;
	last_line_len = 0;
}

int lexer_take(LexemeState *lexeme, FILE *source_file) {
	int c = getc(source_file);
	lexeme->current_lexeme = c;

	if (c == EOF) {
		return EOF;
	}

	// Store the current line and column before updating them
	lexeme->previous_line = current_line;
	lexeme->previous_col = current_col;

	if (c == '\n') {
		last_line_len = current_col; //
		current_line++;
		current_col = 1;
	} else {
		current_col++;
	}

	return c;
}

void lexer_give(LexemeState *lexeme, FILE *source_file) {
	if (lexeme->current_lexeme == EOF) {
		return;
	}

	ungetc(lexeme->current_lexeme, source_file);

	// Restore the previous line and column positions
	current_line = lexeme->previous_line;
	current_col = lexeme->previous_col;
}

Error lexer_get_next_token(Token *out_token) {
	if (!out_token) {
		return ERROR_EMIT(ERR_INTERNAL, current_line, current_col, "Output token pointer is NULL.");
	}

	token_init(out_token);
	LexerState current_state = STATE_START;

	// @dev(Jirka Kolek) Tady budeme implementovat samotnou logiku lexeru => DKA
	out_token->type = TOKEN_EOF;
	out_token->data.str_val = NULL;
	out_token->line = current_line;
	out_token->col = current_col;

	LexemeState lexeme;

	while (true) {
		switch (current_state) {
			case STATE_START:
				lexeme.current_lexeme = lexer_take(&lexeme, source_file);
				if (lexeme.current_lexeme == EOF && ferror(source_file) != 0) {
					return ERROR_EMIT(
						ERR_INTERNAL, current_line, current_col, "Failed to read token from file.");
				}
				if (lexeme.current_lexeme == EOF) {
					out_token->type = TOKEN_EOF;
					out_token->line = current_line;
					out_token->col = current_col;
					return ERROR_OK();
				}
				if (isdigit(lexeme.current_lexeme)) {
					if (lexeme.current_lexeme == '0') {
						current_state = STATE_LEADING_ZERO;
					} else {
						current_state = STATE_NUMBER;
					}
				}
				break;
			// @dev(Jirka Kolek) Bacha na funny písmenka e, E a D, d - viz náčrt DKA
			case STATE_LEADING_ZERO:
				// @dev(Jirka Kolek) Implementace stavu pro číslo začínající nulou -> pokud po nule
				// @dev následuje číslice, je to chyba, pokud následuje tečka, je to float, pokud
				// @dev následuje jiný znak, je to integer 0
				break;

			case STATE_NUMBER:
				// @dev(Jirka Kolek) Implementace stavu pro číslo -> pokud následuje číslice,
				// @dev pokračujeme, pokud následuje tečka, přecházíme do stavu float, pokud
				// @dev následuje
				// @dev jiný znak, je to integer dané číselné hodnoty a vracíš zpátky aktuálně
				// @dev načtený stav
				break;

			case STATE_WANNABE_FLOAT:
				// @dev(Jirka Kolek) Implementace stavu pro float -> pokud následuje číslice,
				// @dev pokračujeme, pokud následuje jiný znak, je to float dané číselné hodnoty a

				// @dev vracíš zpátky aktuálně načtený stav
				break;
			case STATE_ERROR:
				// @dev(Jirka Kolek) Implementace stavu pro chybu
				break;
			default:
				break;
		}
	}

	// @dev(Jirka Kolek) Při průchodu je třeba updateovat current_line a current_col podle toho,
	// @dev kolik znaků jsme přečetli a zda jsme narazili na nový řádek.

	return ERROR_OK();
}
