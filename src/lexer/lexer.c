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
static size_t last_line_len = 0;

static size_t token_start_line = 1; /** Starting line of a token */
static size_t token_start_col = 1; /** Starting column of a token */

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
			last_line_len = current_col; //keep prev line len before reset
			current_col = 1;
			//the moment NEWLINE needs to be a token, I kinda lose the previous col position here??
			return lexeme->current_lexeme;
		case EOF: //possible two entries - real EOF (feof()) and read error (ferror())
			return EOF;
		default:
			current_col++;
			return lexeme->current_lexeme;
	}
}

void lexer_give(LexemeState *lexeme, FILE *source_file) {
	if (lexeme->current_lexeme == EOF) {
		return;
	} //guard against eof, which should NEVER get this far
	current_col = lexeme->previous_col;
	current_line = lexeme->previous_line;
	ungetc(lexeme->current_lexeme, source_file);
}


Error lexer_get_next_token(Token *out_token) {
	if (!out_token) {
		return ERROR_EMIT(ERR_INTERNAL, current_line, current_col, "Output token pointer is NULL.");
	}

	token_init(out_token);

	static LexerState current_state = STATE_START; /** The current state of the lexer. */ //needed, not needed? decide later

	// @dev(Jirka Kolek) Tady budeme implementovat samotnou logiku lexeru => DKA
	for (;;) {
		LexemeState lexeme;
		lexeme.current_lexeme = lexer_take(&lexeme, source_file); //current_lexeme is INT

		if (lexeme.current_lexeme == EOF && ferror(source_file) != 0) {
			return ERROR_EMIT(ERR_INTERNAL, current_line, current_col, "Failed to read token from file.");
		}

		switch (current_state) {
			case STATE_START:
				if (lexeme.current_lexeme == EOF) {
					out_token->type = TOKEN_EOF;
					out_token->data.str_val = NULL;
					out_token->line = current_line;
					out_token->col = current_col;
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == ' ' || lexeme.current_lexeme == '\t') { //not yet solved whitespace logic
					continue;
				}
				if (lexeme.current_lexeme == '(') {
					out_token->type = TOKEN_LPAREN;
					out_token->data.str_val = NULL;
					out_token->line = current_line;
					out_token->col = current_col;
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == '0') {
					token_start_line = current_line;
					token_start_col = current_col;
					current_state = STATE_ZERO_SEEN;
					//add it to token string
					return ERROR_OK();
				}
				if (lexeme.current_lexeme >= '1' && lexeme.current_lexeme <= '9'){
					token_start_line = current_line;
					token_start_col = current_col;
					current_state = STATE_NUMBER_SEEN;
					//add it to token string
					return ERROR_OK();
				}
				//ctype will probably come in handy
				//need to fill in the other possible states, structure comes first
				return ERROR_EMIT(ERR_LEXICAL, current_line, current_col, "Unexpected character.");
			case STATE_NUMBER_SEEN:
				if (lexeme.current_lexeme >= '1' && lexeme.current_lexeme <= '9') {
					//add it to token string
					continue;
				}
				if (lexeme.current_lexeme == '.') {
					current_state = STATE_WANNA_BE_DOUBLE;
					//add it to token string
					continue;
				}
				if (lexeme.current_lexeme == 'e' || lexeme.current_lexeme == 'E') {
					current_state = STATE_WANNA_BE_DOUBLE;
					//add it to token string
					continue;
				}
				if (lexeme.current_lexeme == '_') {
					current_state = STATE_DIV_DEC;
					//add it to token string
					continue;
				}
				// invalid continuation handling here?
				// lexer_give() for returning the invalid read?
				//out_token->type = ???
				out_token->col = token_start_col;
				out_token->line = token_start_line;
				// continue; / return?
			case STATE_ZERO_SEEN:
				continue;
			case STATE_WANNA_BE_DOUBLE:
				continue;
			case STATE_DOUBLE:
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
		}
	}


	// @dev(Jirka Kolek) Při průchodu je třeba updateovat current_line a current_col podle toho,
	// @dev kolik znaků jsme přečetli a zda jsme narazili na nový řádek.

	return ERROR_OK();
}
