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

#include "ast.h"
#include "../_utils/error.h"
#include "../lexer/lexer.h"
#include <stdbool.h>

/**
 * @brief Inner parser flags
 */
typedef enum {
	PAR_SUCC, /**< Successful function return */
	PAR_FAIL, /**< Error while applying rule */
	LEX_FAIL, /**< Error while getting tokens */
	AST_FAIL, /**< Error during allocating AST nodes */
	PAR_EOF	  /**< TOKEN_EOF reached */
} ParserFlag;

/**
 * @brief Structure for passing status between parser functions
 */
typedef struct {
	Error error;	 	/**< Error structure containing error info */
	ParserFlag flag; 	/**< Flag signalizing parser behavior */
	ASTNode* new_node;  /**< Node created by function to be connected to AST tree by caller */
} ParserStatus;

/**
 * @brief Constructor of successful ParserStatus structure when successful
 * @param f Inner parser flag
 * @param n New node to return to caller of function
 */
#define PARSE_OK(f, n) ((ParserStatus){.error = ERROR_OK(), .flag = (f), .new_node = (n)})
/**
 * @brief Constructor of successful ParserStatus structure when successful
 * @param e Error structure
 * @param f Inner parser flag
 */
#define PARSE_ERROR(e, f) ((ParserStatus){.error = (e), .flag = (f)})
/**
 * @brief Initializes parser inner loop
 * @return Returns ParserStatus containing head of AST tree if no error occured
 */
ParserStatus parse();

#endif /* IFJ26_PARSER_H */
