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
 * @brief Inner parser flags
*/
typedef enum {
	PAR_SCAN_ERR,	/**< Error while getting tokens */
	PAR_SUCC,		/**< Successful function return */
	PAR_EOF			/**< TOKEN_EOF reached */
} ParserFlag;

/**
 * @brief Structure for passing status between parser functions
*/
typedef struct {
	Error error;			/**< Error structure containing error info */
	TokenType last_token;		/**< Last token received, used for token consumation */
	ParserFlag flag;		/**< Flag signalizing parser behavior */
} ParserStatus;

/**
 * @brief Constructor of successful ParserStatus structure when successful
 * @param tok Last token received from scanner
*/
#define PARSE_OK(tok) ((ParserStatus){.error = (ERR_OK()), .last_token = (tok), .flag = PAR_SUCC})

/**
 * @brief Constructor of successful ParserStatus structure when successful
 * @param t Type of error
 * @param l Line of error
 * @param c Collumn of error
 * @param msg Error message
 * @param tok Last token received from scanner
*/
#define PARSE_ERROR(t, l, c, msg, tok) ((ParserStatus){.error = (ERR_EMIT((t), (l), (c), (msg))), .last_token = (tok), .flag = PAR_SUCC})

/**
 * @brief Initializes parser inner loop
*/
void parse();

#endif /* IFJ26_PARSER_H */
