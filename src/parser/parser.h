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
#include "../_utils/error.h"
#include <stdbool.h>

/**
 * @brief Inner parser flags
*/
typedef enum {
	PAR_SUCC,		/**< Successful function return */
	PAR_FAIL,		/**< Error while applying rule */
	LEX_FAIL,		/**< Error while getting tokens */
	PAR_EOF			/**< TOKEN_EOF reached */
} ParserFlag;

/**
 * @brief Structure for passing status between parser functions
*/
typedef struct {
	Error error;			/**< Error structure containing error info */
	ParserFlag flag;		/**< Flag signalizing parser behavior */
} ParserStatus;

/**
 * @brief Constructor of successful ParserStatus structure when successful
 * @param flag Inner parser flag
*/
#define PARSE_OK(flag_val) ((ParserStatus){.error = (ERROR_OK()), .flag = (flag_val)})

/**
 * @brief Constructor of successful ParserStatus structure when successful
 * @param err Error structure
 * @param flag Inner parser flag
*/
#define PARSE_ERROR(err, flag_val) ((ParserStatus){.error = (err), .flag = (flag_val)})

/**
 * @brief Initializes parser inner loop
*/
void parse();

#endif /* IFJ26_PARSER_H */
