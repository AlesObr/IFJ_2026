/**
 * @project IFJ26 Compiler
 * @file precedence.h
 * @brief Precedence expression parsing interface for parser
 * @author Štěpán Hořenek (xhorens00)
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_PRECEDENCE_H
#define IFJ26_PRECEDENCE_H

#include "../_utils/error.h"
#include "../_utils/token.h"
#include "../lexer/lexer.h"
#include "ast.h"
#include "parser.h"
#include <stdbool.h>

/**
 * @brief Function for parsing of expressions and conditions
 * @param curr_token Token structure with last token fetched from scanner
 * @return Status structure to be returned to caller
 */
ParserStatus parse_expression(Token *curr_token);

#endif /* IFJ26_PRECEDENCE_H */
