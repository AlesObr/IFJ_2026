/**
 * @project IFJ26 Compiler
 * @file parser.h
 * @brief Syntactic analyzer and recursive descent interface.
 * @author Štěpán Hořenek (xhorens00)
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_PARSER_H
#define IFJ26_PARSER_H

#include "../lexer/lexer.h"
#include <stdbool.h>

/**
 * @brief Initializes parser inner loop
*/
void parse();

#endif /* IFJ26_PARSER_H */
