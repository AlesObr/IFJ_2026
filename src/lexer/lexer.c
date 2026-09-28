/**
 * @project IFJ26 Compiler
 * @file lexer.c
 * @brief Lexer module implementation for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @author Admiral (xkolekj00)
 * @note Standard: ISO C11
 */

#include "lexer.h"
#include "../_utils/dstring.h"
#include "../_utils/error.h"
#include "../_utils/token.h"
#include <stdio.h>
#include <stdlib.h>

/*All Global vars. Below are the important ones to keep current state of Lexer */
static FILE *source_file = NULL; /** The input file being lexed - it will be de facto STDIN */
static size_t current_line = 1;	 /** The current line number in the source file being lexed */
static size_t current_col = 1;	 /** The current column number in the source file being lexed */
static size_t last_line_len = 0; /** The length of the previous line */

static size_t token_start_line = 1; /** Starting line of a token */
static size_t token_start_col = 1;	/** Starting column of a token */

void lexer_init(FILE *input) {
	source_file = input;
	current_line = 1;
	current_col = 1;
	last_line_len = 0;
}

int lexer_take(LexemeState *lexeme, FILE *source_file) {
	lexeme->current_lexeme = getc(source_file);
	lexeme->previous_col = current_col;
	lexeme->previous_line = current_line;

	switch (lexeme->current_lexeme) {
		case '\n':
			current_line++;
			last_line_len = current_col; // keep prev line len before reset
			current_col = 1;
			// the moment NEWLINE needs to be a token, I kinda lose the previous col position here??
			return lexeme->current_lexeme;
		case EOF: // possible two entries - real EOF (feof()) and read error (ferror())
			return EOF;
		default:
			current_col++;
			return lexeme->current_lexeme;
	}
}

void lexer_give(LexemeState *lexeme, FILE *source_file) {
	if (lexeme->current_lexeme == EOF) {
		return;
	} // guard against eof, which should NEVER get this far
	current_col = lexeme->previous_col;
	current_line = lexeme->previous_line;
	ungetc(lexeme->current_lexeme, source_file);
}

Error lexer_get_next_token(Token *out_token) {
	if (!out_token) {
		return ERROR_EMIT(ERR_INTERNAL, current_line, current_col, "Output token pointer is NULL.");
	}

	token_init(out_token);

	LexerState current_state = STATE_START; /** The current state of the lexer. */ //@dev needed
	dstring_t token_buffer;
	if (!dstring_init(&token_buffer)) {
		return ERROR_EMIT(
			ERR_INTERNAL, current_line, current_col, "Failed to initialize token buffer.");
	}

	// @dev(Jirka Kolek) Tady budeme implementovat samotnou logiku lexeru => DKA
	for (;;) {
		LexemeState lexeme;

		switch (current_state) {
			case STATE_START:
				lexeme.current_lexeme = lexer_take(&lexeme, source_file);

				if (lexeme.current_lexeme == EOF && ferror(source_file) != 0) {
					dstring_free(&token_buffer);
					return ERROR_EMIT(
						ERR_INTERNAL, current_line, current_col, "Failed to read token from file.");
				} // will probably need to be somehow incorporated into lexer_take
				if (lexeme.current_lexeme == EOF) {
					token_make_simple(out_token, TOKEN_EOF, current_line, current_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				//@dev isspace() will probably come in handy for whitespace handling
				if (lexeme.current_lexeme == ' ' ||
					lexeme.current_lexeme == '\t') { // not yet solved whitespace logic
					continue;
				}
				if (lexeme.current_lexeme == '(') {
					token_make_simple(out_token, TOKEN_LPAREN, current_line, current_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == ')') {
					token_make_simple(out_token, TOKEN_RPAREN, current_line, current_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == '0') {
					token_start_line = current_line;
					token_start_col = current_col;
					current_state = STATE_ZERO_SEEN;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
					// @dev tady return být nesmí
				}
				if (lexeme.current_lexeme >= '1' && lexeme.current_lexeme <= '9') {
					token_start_line = current_line;
					token_start_col = current_col;
					current_state = STATE_NUMBER_SEEN;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
					// @dev return tady být nesmí
				}
				// ctype will probably come in handy
				// need to fill in the other possible states, structure comes first
				dstring_free(&token_buffer);
				return ERROR_EMIT(ERR_LEXICAL, current_line, current_col, "Unexpected character.");
			case STATE_NUMBER_SEEN:
				if (lexeme.current_lexeme >= '1' && lexeme.current_lexeme <= '9') {
					// add it to token string
					continue;
				}
				if (lexeme.current_lexeme == '.') {
					current_state = STATE_WANNA_BE_DOUBLE;
					// add it to token string
					continue;
				}
				if (lexeme.current_lexeme == 'e' || lexeme.current_lexeme == 'E') {
					current_state = STATE_WANNA_BE_DOUBLE;
					// add it to token string
					continue;
				}
				if (lexeme.current_lexeme == '_') {
					current_state = STATE_DIV_DEC;
					// add it to token string
					continue;
				}
				// invalid continuation handling here?
				// lexer_give() for returning the invalid read?
				// out_token->type = ???
				out_token->col = token_start_col;
				out_token->line = token_start_line;

				// @dev(Jirka Kolek) Implementace stavu pro číslo -> pokud následuje číslice,
				// @dev pokračujeme, pokud následuje tečka, přecházíme do stavu float, pokud
				// @dev následuje
				// @dev jiný znak, je to integer dané číselné hodnoty a vracíš zpátky aktuálně
				// @dev načtený stav

				// continue; / return?
			case STATE_ZERO_SEEN: // renamed to LEADING_ZERO???
				// @dev(Jirka Kolek) Implementace stavu pro číslo začínající nulou -> pokud po nule
				// @dev následuje číslice, je to chyba, pokud následuje tečka, je to float, pokud
				// @dev následuje jiný znak, je to integer 0
				continue;
			case STATE_WANNA_BE_DOUBLE:
				continue;
			case STATE_DOUBLE:

				// @dev(Jirka Kolek) Implementace stavu pro float -> pokud následuje číslice,
				// @dev pokračujeme, pokud následuje jiný znak, je to float dané číselné hodnoty a
				// @dev vracíš zpátky aktuálně načtený stav

				continue;
			case STATE_DIV_DEC:
				continue;
			case STATE_UNDER_SCORE:
				continue;
			case STATE_WANNA_BE_EXP_1:
				continue;
			case STATE_WANNA_BE_EXP_2:
				continue;
			case STATE_EXP_SIGN_1:
				continue;
			case STATE_EXP_SIGN_2:
				continue;
			case STATE_EXPONENT:
				continue;
			case STATE_SUFFIX:
				continue;
			default:
				break;

				// @dev(Jirka Kolek) Implementace stavu pro chybu
		}
	}

	// @dev(Jirka Kolek) Při průchodu je třeba updateovat current_line a current_col podle toho,
	// @dev kolik znaků jsme přečetli a zda jsme narazili na nový řádek.

	return ERROR_OK();
}
