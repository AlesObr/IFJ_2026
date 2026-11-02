/**
 * @project IFJ26 Compiler
 * @file precedence.c
 * @brief Precedence expression parsing interface for parser
 * @author Štěpán Hořenek (xhorens00)
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#include "precedence.h"
#include "../_utils/error.h"
#include "../_utils/token.h"
#include "../lexer/lexer.h"
#include "ast.h"
#include "parser.h"
#include <stdbool.h>

ParserStatus parse_expression(Token *curr_token) {
	(void)curr_token;
	return PARSE_ERROR(
		ERROR_EMIT(ERR_INTERNAL, 0, 0, "Expression parser is not implemented\n"), AST_FAIL);
}
