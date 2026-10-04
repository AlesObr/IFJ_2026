/**
 * @project IFJ26 Compiler
 * @file token.c
 * @brief Token module implementation for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 *
 * @par Ownership contract
 * Functions receiving a Token pointer borrow the instance; they do NOT take ownership.
 * The caller retains full ownership and is strictly responsible for calling token_destroy()
 * when the token is no longer needed.
 *
 * If any operation fails, the Token is guaranteed to remain in a valid, destructible state.
 *
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
		free(t->data.str_val);
		t->data.str_val = NULL;
	}
}
