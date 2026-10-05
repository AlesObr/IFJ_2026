#include "../../src/lexer/lexer.h"
#include "../../src/_utils/token.h"
#include "../../src/_utils/error.h"
#include "../../src/_utils/dstring.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

static const char *token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_EOF:             return "EOF";
        case TOKEN_IDENTIFIER:      return "IDENTIFIER";
        case TOKEN_KEYWORD_DEF:     return "KEYWORD:def";
        case TOKEN_KEYWORD_DO:      return "KEYWORD:do";
        case TOKEN_KEYWORD_CLASS:   return "KEYWORD:class";
        case TOKEN_KEYWORD_CASE:    return "KEYWORD:case";
        case TOKEN_KEYWORD_EXT:     return "KEYWORD:ext";
        case TOKEN_KEYWORD_TYPE:    return "KEYWORD:type";
        case TOKEN_KEYWORD_MATCH:   return "KEYWORD:match";
        case TOKEN_KEYWORD_BOOLEAN: return "KEYWORD:boolean";
        case TOKEN_KEYWORD_LIST:    return "KEYWORD:list";
        case TOKEN_KEYWORD_FOR:     return "KEYWORD:for";
        case TOKEN_KEYWORD_INT:     return "KEYWORD:int";
        case TOKEN_KEYWORD_DOUBLE:  return "KEYWORD:Double";
        case TOKEN_KEYWORD_STRING:  return "KEYWORD:String";
        case TOKEN_KEYWORD_UNIT:    return "KEYWORD:Unit";
        case TOKEN_KEYWORD_VAR:     return "KEYWORD:var";
        case TOKEN_KEYWORD_VAL:     return "KEYWORD:val";
        case TOKEN_KEYWORD_IF:      return "KEYWORD:if";
        case TOKEN_KEYWORD_ELSE:    return "KEYWORD:else";
        case TOKEN_KEYWORD_WHILE:   return "KEYWORD:while";
        case TOKEN_KEYWORD_THEN:    return "KEYWORD:then";
        case TOKEN_KEYWORD_IMPORT:  return "KEYWORD:import";
        case TOKEN_KEYWORD_RETURN:  return "KEYWORD:return";
        case TOKEN_KEYWORD_NULL:    return "KEYWORD:null";
        case TOKEN_KEYWORD_TRUE:    return "KEYWORD:true";
        case TOKEN_KEYWORD_FALSE:   return "KEYWORD:false";
        case TOKEN_LIT_INT:         return "LIT_INT";
        case TOKEN_LIT_FLOAT:       return "LIT_FLOAT";
        case TOKEN_LIT_STRING:      return "LIT_STRING";
        case TOKEN_PLUS:            return "PLUS";
        case TOKEN_MINUS:           return "MINUS";
        case TOKEN_MUL:             return "MUL";
        case TOKEN_DIV:             return "DIV";
        case TOKEN_DOT:             return "DOT";
	    case TOKEN_AT:              return "AT";
        case TOKEN_EOL:             return "EOL";
        case TOKEN_INDENT:          return "INDENT";
        case TOKEN_UNDERSCORE:      return "UNDERSCORE";
        case TOKEN_COLON:           return "COLON";
        case TOKEN_ASSIGN:          return "ASSIGN";
        case TOKEN_EQ:              return "EQ";
        case TOKEN_NEQ:             return "NEQ";
        case TOKEN_LT:              return "LT";
        case TOKEN_LTE:             return "LTE";
        case TOKEN_GT:              return "GT";
        case TOKEN_GTE:             return "GTE";
        case TOKEN_AND:             return "AND";
        case TOKEN_OR:              return "OR";
        case TOKEN_NOT:             return "NOT";
        case TOKEN_LPAREN:          return "LPAREN";
        case TOKEN_RPAREN:          return "RPAREN";
        case TOKEN_LBRACE:          return "LBRACE";
        case TOKEN_RBRACE:          return "RBRACE";
        case TOKEN_LBRACKET:        return "LBRACKET";
        case TOKEN_RBRACKET:        return "RBRACKET";
        case TOKEN_SEMICOLON:       return "SEMICOLON";
        case TOKEN_COMMA:           return "COMMA";
    	case TOKEN_PIPE:            return "PIPE";
        default:                    return "UNKNOWN";
    }
}

int main(void) {
    lexer_init(stdin);

    Token token;
    Error err;

    do {
        err = lexer_get_next_token(&token);
        if (err.type != ERR_OK) {
            token_destroy(&token);
            return err.type;
        }

        switch (token.type) {
            case TOKEN_IDENTIFIER:
                printf("%zu:IDENTIFIER:%s\n", token.line, token.data.str_val ? dstring_get(token.data.str_val) : "");
                break;

            case TOKEN_LIT_STRING:
                printf("%zu:LIT_STRING:%s\n", token.line, token.data.str_val ? dstring_get(token.data.str_val) : "");
                break;

            case TOKEN_LIT_INT:
                printf("%zu:LIT_INT:%" PRId64 "\n", token.line, token.data.int_val);
                break;

            case TOKEN_LIT_FLOAT:
                printf("%zu:LIT_FLOAT:%g\n", token.line, token.data.float_val);
                break;

            case TOKEN_INDENT:
                printf("%zu:INDENT:%zu\n", token.line, (size_t)token.data.int_val);
                break;

            default:
                printf("%zu:%s\n", token.line, token_type_to_string(token.type));
                break;
        }

        TokenType last_type = token.type;
        token_destroy(&token);

        if (last_type == TOKEN_EOF) {
            break;
        }

    } while (1);

    return 0;
}
