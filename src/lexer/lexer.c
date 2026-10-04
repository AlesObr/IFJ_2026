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
#include <string.h>

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
			last_line_len = current_col;
			current_col = 1;
			return lexeme->current_lexeme;
		case EOF: //** Possible two entries - real EOF (feof()) and read error (ferror()) */
			return EOF;
		default:
			current_col++;
			return lexeme->current_lexeme;
	}
}

void lexer_give(LexemeState *lexeme, FILE *source_file) {
	if (lexeme->current_lexeme == EOF) {
		return;
	} // Guard against eof, which should NEVER get this far
	current_col = lexeme->previous_col;
	current_line = lexeme->previous_line;
	ungetc(lexeme->current_lexeme, source_file);
}

Error lexer_get_next_token(Token *out_token) {
	if (!out_token) {
		return ERROR_EMIT(ERR_INTERNAL, current_line, current_col, "Output token pointer is NULL.");
	}

	token_init(out_token);

	LexerState current_state = STATE_START;
	dstring_t token_buffer;
	if (!dstring_init(&token_buffer)) {
		return ERROR_EMIT(
			ERR_INTERNAL, current_line, current_col, "Failed to initialize token buffer.");
	}

	for (;;) {
		LexemeState lexeme;
		lexeme.current_lexeme = lexer_take(&lexeme, source_file);

		switch (current_state) {
			case STATE_START:

				if (lexeme.current_lexeme == EOF && ferror(source_file) != 0) {
					dstring_free(&token_buffer);
					return ERROR_EMIT(
						ERR_INTERNAL, current_line, current_col, "Failed to read token from file.");
				} // @dev will probably need to be somehow incorporated into lexer_take
				if (lexeme.current_lexeme == EOF) {
					token_make_simple(out_token, TOKEN_EOF, current_line, current_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				// @dev isspace() will probably come in handy for whitespace handling
				if (lexeme.current_lexeme == ' ' ||
					lexeme.current_lexeme == '\t') { // @dev not yet solved whitespace logic
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
				}
				if (lexeme.current_lexeme >= '1' && lexeme.current_lexeme <= '9') {
					token_start_line = current_line;
					token_start_col = current_col;
					current_state = STATE_NUMBER_SEEN;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				if (lexeme.current_lexeme == '_') {
					token_start_line = current_line;
					token_start_col = current_col;
					current_state = STATE_DIV_DEC;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				if (lexeme.current_lexeme == '.') {
					token_start_line = current_line;
					token_start_col = current_col;
					current_state = STATE_WANNA_BE_DOUBLE;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				dstring_free(&token_buffer);
				return ERROR_EMIT(ERR_LEXICAL, current_line, current_col, "Unexpected character.");

			case STATE_NUMBER_SEEN:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				if (lexeme.current_lexeme == '.') {
					current_state = STATE_WANNA_BE_DOUBLE;
					dstring_append_char(&token_buffer, '.');
					continue;
				}
				if (lexeme.current_lexeme == 'e' || lexeme.current_lexeme == 'E') {
					current_state = STATE_WANNA_BE_EXP_1;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				if (lexeme.current_lexeme == '_') {
					current_state = STATE_DIV_DEC;
					continue;
				}

				// Return the integer token if we reach a non-digit character
				lexer_give(&lexeme, source_file);
				int64_t int_value = (int64_t)strtoll(dstring_get(&token_buffer), NULL, 10);
				token_make_int(out_token, int_value, token_start_line, token_start_col);
				dstring_free(&token_buffer);
				return ERROR_OK();
				// @dev(Jirka Kolek) Implementace stavu pro číslo -> pokud následuje číslice,
				// @dev pokračujeme, pokud následuje tečka, přecházíme do stavu float, pokud
				// @dev následuje
				// @dev jiný znak, je to integer dané číselné hodnoty a vracíš zpátky aktuálně
				// @dev načtený stav
			case STATE_ZERO_SEEN: //@dev renamed to LEADING_ZERO???
				// @dev(Jirka Kolek) Implementace stavu pro číslo začínající nulou -> pokud po nule
				// @dev následuje číslice, je to chyba, pokud následuje tečka, je to float, pokud
				// @dev následuje jiný znak, je to integer 0
				if (lexeme.current_lexeme == '.') {
					current_state = STATE_WANNA_BE_DOUBLE;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				if (lexeme.current_lexeme == 'e' || lexeme.current_lexeme == 'E') {
					current_state = STATE_WANNA_BE_EXP_1;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				if (lexeme.current_lexeme == '_') {
					current_state = STATE_DIV_DEC;
					continue;
				} // @dev toto tu dle dka být nemá ?
				lexer_give(&lexeme, source_file);
				token_make_int(out_token, lexeme.current_lexeme, token_start_line, token_start_col); // lexeme.current_lexeme ? 0
				dstring_free(&token_buffer);
				return ERROR_OK();

				continue;
			case STATE_WANNA_BE_DOUBLE:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					current_state = STATE_DOUBLE;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				if (lexeme.current_lexeme == EOF){
					dstring_free(&token_buffer);
					return ERROR_EMIT(ERR_LEXICAL, current_line, current_col, "Malformed number followed by EOF.");
				}
				continue;
			case STATE_DOUBLE:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				if (lexeme.current_lexeme == '_') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					current_state = STATE_UNDER_SCORE;
					continue;
				}

				// Exponent
				if (lexeme.current_lexeme == 'e' || lexeme.current_lexeme == 'E') {
					current_state = STATE_WANNA_BE_EXP_2;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}

				if (lexeme.current_lexeme == 'd' || lexeme.current_lexeme == 'D') {
					current_state = STATE_SUFFIX;
					// We don't add the suffix to the buffer, as strtod will ignore it and we want
					// to keep the value clean
					continue;
				}

				lexer_give(&lexeme, source_file);
				double double_val = strtod(dstring_get(&token_buffer), NULL);
				token_make_float(out_token, double_val, token_start_line, token_start_col);
				dstring_free(&token_buffer);
				return ERROR_OK();
				// @dev(Jirka Kolek) Implementace stavu pro float -> pokud následuje číslice,
				// @dev pokračujeme, pokud následuje jiný znak, je to float dané číselné hodnoty a
				// @dev vracíš zpátky aktuálně načtený stav

				continue;
			case STATE_DIV_DEC:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					current_state = STATE_NUMBER_SEEN;
					continue;
				}
				if (lexeme.current_lexeme == EOF)
					return ERROR_EMIT(ERR_LEXICAL, current_line, current_col, "Malformed number followed by EOF.");
				continue;
			case STATE_UNDER_SCORE:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					current_state = STATE_DOUBLE;
					continue;
				}
				else //if (lexeme.current_lexeme == EOF)
					return ERROR_EMIT(ERR_LEXICAL, current_line, current_col, "Malformed number followed by EOF.");
				continue;
			case STATE_WANNA_BE_EXP_1:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					// @dev printf("%s", token_buffer.data);
					current_state = STATE_EXPONENT;
					continue;
				}
				if (lexeme.current_lexeme == '+' || lexeme.current_lexeme == '-') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					// @dev printf("%s", token_buffer.data);
					current_state = STATE_EXP_SIGN_1;
					continue;
				}
				if (lexeme.current_lexeme == EOF)
					return ERROR_EMIT(ERR_LEXICAL, current_line, current_col, "Malformed number followed by EOF.");
				continue;
			case STATE_WANNA_BE_EXP_2:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					// @dev printf("%s", token_buffer.data);
					current_state = STATE_EXPONENT;
					continue;
				}
				if (lexeme.current_lexeme == '+' || lexeme.current_lexeme == '-') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					// @dev printf("%s", token_buffer.data);
					current_state = STATE_EXP_SIGN_2;
					continue;
				}
				if (lexeme.current_lexeme == EOF)
					return ERROR_EMIT(ERR_LEXICAL, current_line, current_col, "Malformed number followed by EOF.");
				continue;
			case STATE_EXP_SIGN_1:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					current_state = STATE_EXPONENT;
					continue;
				}
				if (lexeme.current_lexeme == EOF) {
					dstring_free(&token_buffer);
					return ERROR_EMIT(ERR_LEXICAL, current_line, current_col, "Malformed number followed by EOF.");
				}
				continue;
			case STATE_EXP_SIGN_2:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					current_state = STATE_EXPONENT;
					continue;
				}
				if (lexeme.current_lexeme == EOF){
					dstring_free(&token_buffer);
					return ERROR_EMIT(ERR_LEXICAL, current_line, current_col, "Malformed number followed by EOF.");
				}
				continue;
			case STATE_EXPONENT:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
					// @dev exponent je koncový stav dka
				}
				if (lexeme.current_lexeme == EOF)
					return ERROR_OK();
				continue;
			case STATE_SUFFIX:
				lexer_give(&lexeme, source_file);
				token_make_float(out_token, strtod(dstring_get(&token_buffer), NULL),
					token_start_line, token_start_col);
				dstring_free(&token_buffer);
				return ERROR_OK();
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

TokenType is_it_keyword(const char *str) {
	if (strcmp(str, "def") == 0) {
		return TOKEN_KEYWORD_DEF;
	} else if (strcmp(str, "do") == 0) {
		return TOKEN_KEYWORD_DO;
	} else if (strcmp(str, "class") == 0) {
		return TOKEN_KEYWORD_CLASS;
	} else if (strcmp(str, "case") == 0) {
		return TOKEN_KEYWORD_CASE;
	} else if (strcmp(str, "ext") == 0) {
		return TOKEN_KEYWORD_EXT;
	} else if (strcmp(str, "type") == 0) {
		return TOKEN_KEYWORD_TYPE;
	} else if (strcmp(str, "match") == 0) {
		return TOKEN_KEYWORD_MATCH;
	} else if (strcmp(str, "boolean") == 0) {
		return TOKEN_KEYWORD_BOOLEAN;
	} else if (strcmp(str, "list") == 0) {
		return TOKEN_KEYWORD_LIST;
	} else if (strcmp(str, "for") == 0) {
		return TOKEN_KEYWORD_FOR;
	} else if (strcmp(str, "int") == 0) {
		return TOKEN_KEYWORD_INT;
	} else if (strcmp(str, "double") == 0) {
		return TOKEN_KEYWORD_DOUBLE;
	} else if (strcmp(str, "string") == 0) {
		return TOKEN_KEYWORD_STRING;
	} else if (strcmp(str, "unit") == 0) {
		return TOKEN_KEYWORD_UNIT;
	} else if (strcmp(str, "var") == 0) {
		return TOKEN_KEYWORD_VAR;
	} else if (strcmp(str, "val") == 0) {
		return TOKEN_KEYWORD_VAL;
	} else if (strcmp(str, "if") == 0) {
		return TOKEN_KEYWORD_IF;
	} else if (strcmp(str, "else") == 0) {
		return TOKEN_KEYWORD_ELSE;
	} else if (strcmp(str, "while") == 0) {
		return TOKEN_KEYWORD_WHILE;
	} else if (strcmp(str, "then") == 0) {
		return TOKEN_KEYWORD_THEN;
	} else if (strcmp(str, "import") == 0) {
		return TOKEN_KEYWORD_IMPORT;
	} else if (strcmp(str, "return") == 0) {
		return TOKEN_KEYWORD_RETURN;
	} else if (strcmp(str, "null") == 0) {
		return TOKEN_KEYWORD_NULL;
	} else if (strcmp(str, "true") == 0) {
		return TOKEN_KEYWORD_TRUE;
	} else if (strcmp(str, "false") == 0) {
		return TOKEN_KEYWORD_FALSE;
	}
	return TOKEN_IDENTIFIER;
}
