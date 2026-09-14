/**
 * @file token.h
 * @brief Token module interface for IFJ26 compiler.
 * @author Aleš Obr (skolp)
 * @note Standard: ISO C11
 */


typedef struct token
 {

 };

 typedef enum token_type
 {
	TOKEN_TYPE_IDENTIFIER,
	TOKEN_TYPE_KEYWORD,
	TOKEN_TYPE_LITERAL,
	TOKEN_TYPE_OPERATOR,
	TOKEN_TYPE_SEPARATOR
 };

 /**
  * @brief Initializes a token structure.
  */
 void token_init(struct token *t);
