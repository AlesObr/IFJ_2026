/**
 * @project IFJ26 Compiler
 * @file lexer.c
 * @brief Lexer module implementation for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @author Jirka Kolek (xkolekj00)
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
static LexerState current_state = STATE_START; /** The current state of the lexer. */

static FILE *source_file = NULL; /** The input file being lexed - it will be de facto STDIN */
static size_t current_line = 1;	 /** The current line number in the source file being lexed */
static size_t current_col = 1;	 /** The current column number in the source file being lexed */

void lexer_init(FILE *input) {
	source_file = input;
	current_line = 1;
}

int lexer_take(LexemeState *lexeme, FILE *source_file) {
	lexeme->current_lexeme = getc(source_file);
	lexeme->previous_col = current_col;
	lexeme->previous_line = current_line;
	current_col++;
	switch (lexeme->current_lexeme) {
		case NEWLINE:
			current_line++;
			current_col = 0;
			//the moment NEWLINE needs to be a token, I kinda lose the previous col position here??
			return lexeme->current_lexeme;
		case EOF: //possible two entries - real EOF (feof()) and read error (ferror())
			return EOF;
		default:
			return lexeme->current_lexeme;
	}
}

void lexer_give(LexemeState *lexeme, FILE *source_file) {
	current_col = lexeme->previous_col;
	current_line = lexeme->previous_line;
	ungetc(lexeme->current_lexeme, source_file);
}


Error lexer_get_next_token(Token *out_token) {
	if (!out_token) {
		return ERROR_EMIT(ERR_INTERNAL, current_line, current_col, "Output token pointer is NULL.");
	}

	token_init(out_token);

	// @dev(Jirka Kolek) Tady budeme implementovat samotnou logiku lexeru => DKA
	out_token->type = TOKEN_EOF;
	out_token->data.str_val = NULL;
	out_token->line = current_line;
	out_token->col = current_col;

	LexemeState lexeme;
	lexeme.current_lexeme = lexer_take(&lexeme, source_file);
	if (lexeme.current_lexeme == EOF && ferror(source_file) != 0) {
		return ERROR_EMIT(ERR_INTERNAL, current_line, current_col, "Failed to read token from file.");
	}

	// @dev(Jirka Kolek) Při průchodu je třeba updateovat current_line a current_col podle toho,
	// @dev kolik znaků jsme přečetli a zda jsme narazili na nový řádek.

	return ERROR_OK();
}
