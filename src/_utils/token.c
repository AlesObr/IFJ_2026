/**
 * @project IFJ26 Compiler
 * @file token.c
 * @brief Token module implementation for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @author Admiral (xkolekj00)
 * @note Standard: ISO C11
 */

#include "token.h"
#include "../_utils/error.h"
#include <stdlib.h>

void token_init(Token *t) {
	if (!t)
		return;
	t->type = TOKEN_EOF;
	t->data.str_val = NULL;
	t->line = 1;
	t->col = 1;
}

void token_destroy(Token *t) {
	if (!t)
		return;
	if ((t->type == TOKEN_IDENTIFIER || t->type == TOKEN_LIT_STRING) && t->data.str_val != NULL) {
		dstring_free(t->data.str_val);
		free(t->data.str_val);
		t->data.str_val = NULL;
	}
}
