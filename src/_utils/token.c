/**
 * @project IFJ26 Compiler
 * @file token.c
 * @brief Token module implementation for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @author Jiří Kolek (xkolekj00)
 * @author Štěpán Hořenek (xhorens00)
 * @note Standard: ISO C11
 */

#include "token.h"
#include "../_utils/error.h"
#include <stdlib.h>

const char *token_type_name(TokenType type) {
	switch (type) {
		case TOKEN_EOF:
			return "TOKEN_EOF";
		case TOKEN_IDENTIFIER:
			return "TOKEN_IDENTIFIER";
		case TOKEN_KEYWORD_DEF:
			return "TOKEN_KEYWORD_DEF";
		case TOKEN_KEYWORD_DO:
			return "TOKEN_KEYWORD_DO";
		case TOKEN_KEYWORD_CLASS:
			return "TOKEN_KEYWORD_CLASS";
		case TOKEN_KEYWORD_CASE:
			return "TOKEN_KEYWORD_CASE";
		case TOKEN_KEYWORD_EXTENSION:
			return "TOKEN_KEYWORD_EXTENSION";
		case TOKEN_KEYWORD_TYPE:
			return "TOKEN_KEYWORD_TYPE";
		case TOKEN_KEYWORD_MATCH:
			return "TOKEN_KEYWORD_MATCH";
		case TOKEN_KEYWORD_BOOLEAN:
			return "TOKEN_KEYWORD_BOOLEAN";
		case TOKEN_KEYWORD_LIST:
			return "TOKEN_KEYWORD_LIST";
		case TOKEN_KEYWORD_FOR:
			return "TOKEN_KEYWORD_FOR";
		case TOKEN_KEYWORD_INT:
			return "TOKEN_KEYWORD_INT";
		case TOKEN_KEYWORD_DOUBLE:
			return "TOKEN_KEYWORD_DOUBLE";
		case TOKEN_KEYWORD_STRING:
			return "TOKEN_KEYWORD_STRING";
		case TOKEN_KEYWORD_UNIT:
			return "TOKEN_KEYWORD_UNIT";
		case TOKEN_KEYWORD_VAR:
			return "TOKEN_KEYWORD_VAR";
		case TOKEN_KEYWORD_VAL:
			return "TOKEN_KEYWORD_VAL";
		case TOKEN_KEYWORD_IF:
			return "TOKEN_KEYWORD_IF";
		case TOKEN_KEYWORD_ELSE:
			return "TOKEN_KEYWORD_ELSE";
		case TOKEN_KEYWORD_WHILE:
			return "TOKEN_KEYWORD_WHILE";
		case TOKEN_KEYWORD_THEN:
			return "TOKEN_KEYWORD_THEN";
		case TOKEN_KEYWORD_IMPORT:
			return "TOKEN_KEYWORD_IMPORT";
		case TOKEN_KEYWORD_RETURN:
			return "TOKEN_KEYWORD_RETURN";
		case TOKEN_KEYWORD_TRUE:
			return "TOKEN_KEYWORD_TRUE";
		case TOKEN_KEYWORD_FALSE:
			return "TOKEN_KEYWORD_FALSE";
		case TOKEN_KEYWORD_TO:
			return "TOKEN_KEYWORD_TO";
		case TOKEN_KEYWORD_BY:
			return "TOKEN_KEYWORD_BY";
		case TOKEN_KEYWORD_UNTIL:
			return "TOKEN_KEYWORD_UNTIL";
		case TOKEN_LIT_INT:
			return "TOKEN_LIT_INT";
		case TOKEN_LIT_FLOAT:
			return "TOKEN_LIT_FLOAT";
		case TOKEN_LIT_STRING:
			return "TOKEN_LIT_STRING";
		case TOKEN_PLUS:
			return "TOKEN_PLUS";
		case TOKEN_MINUS:
			return "TOKEN_MINUS";
		case TOKEN_MUL:
			return "TOKEN_MUL";
		case TOKEN_DIV:
			return "TOKEN_DIV";
		case TOKEN_DOT:
			return "TOKEN_DOT";
		case TOKEN_AT_MAIN:
			return "TOKEN_AT_MAIN";
		case TOKEN_EOL:
			return "TOKEN_EOL";
		case TOKEN_INDENT:
			return "TOKEN_INDENT";
		case TOKEN_OUTDENT:
			return "TOKEN_OUTDENT";
		case TOKEN_UNDERSCORE:
			return "TOKEN_UNDERSCORE";
		case TOKEN_COLON:
			return "TOKEN_COLON";
		case TOKEN_ASSIGN:
			return "TOKEN_ASSIGN";
		case TOKEN_EQ:
			return "TOKEN_EQ";
		case TOKEN_NEQ:
			return "TOKEN_NEQ";
		case TOKEN_LT:
			return "TOKEN_LT";
		case TOKEN_LTE:
			return "TOKEN_LTE";
		case TOKEN_GT:
			return "TOKEN_GT";
		case TOKEN_GTE:
			return "TOKEN_GTE";
		case TOKEN_AND:
			return "TOKEN_AND";
		case TOKEN_OR:
			return "TOKEN_OR";
		case TOKEN_PIPE:
			return "TOKEN_PIPE";
		case TOKEN_NOT:
			return "TOKEN_NOT";
		case TOKEN_LPAREN:
			return "TOKEN_LPAREN";
		case TOKEN_RPAREN:
			return "TOKEN_RPAREN";
		case TOKEN_LBRACE:
			return "TOKEN_LBRACE";
		case TOKEN_RBRACE:
			return "TOKEN_RBRACE";
		case TOKEN_LBRACKET:
			return "TOKEN_LBRACKET";
		case TOKEN_RBRACKET:
			return "TOKEN_RBRACKET";
		case TOKEN_SEMICOLON:
			return "TOKEN_SEMICOLON";
		case TOKEN_COMMA:
			return "TOKEN_COMMA";
		default:
			return "TOKEN_UNKNOWN";
	}
}

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
