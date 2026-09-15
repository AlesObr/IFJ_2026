#include "../../src/lexer/lexer.h"
#include "../../src/_utils/token.h"
#include <stdio.h>
#include <stdlib.h>

static const char *token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_EOF:             return "EOF";
        case TOKEN_IDENTIFIER:      return "IDENTIFIER";
        case TOKEN_KEYWORD_DEF:     return "KEYWORD:def";
        case TOKEN_KEYWORD_VAR:     return "KEYWORD:var";
        case TOKEN_KEYWORD_VAL:     return "KEYWORD:val";
        case TOKEN_KEYWORD_IF:      return "KEYWORD:if";
        case TOKEN_KEYWORD_ELSE:    return "KEYWORD:else";
        case TOKEN_KEYWORD_WHILE:   return "KEYWORD:while";
        case TOKEN_KEYWORD_RETURN:  return "KEYWORD:return";
        case TOKEN_LIT_INT:         return "LIT_INT";
        case TOKEN_LIT_FLOAT:       return "LIT_FLOAT";
        case TOKEN_LIT_STRING:      return "LIT_STRING";
        case TOKEN_PLUS:            return "PLUS";
        case TOKEN_MINUS:           return "MINUS";
        case TOKEN_MUL:             return "MUL";
        case TOKEN_DIV:             return "DIV";
        case TOKEN_ASSIGN:          return "ASSIGN";
        case TOKEN_EQ:              return "EQ";
        case TOKEN_NEQ:             return "NEQ";
        case TOKEN_LT:              return "LT";
        case TOKEN_LTE:             return "LTE";
        case TOKEN_GT:              return "GT";
        case TOKEN_GTE:             return "GTE";
        case TOKEN_LPAREN:          return "LPAREN";
        case TOKEN_RPAREN:          return "RPAREN";
        case TOKEN_LBRACE:          return "LBRACE";
        case TOKEN_RBRACE:          return "RBRACE";
        case TOKEN_COLON:           return "COLON";
        case TOKEN_SEMICOLON:       return "SEMICOLON";
        case TOKEN_COMMA:           return "COMMA";
        default:                    return "UNKNOWN";
    }
}

int main(void) {
    lexer_init(stdin);

    Token token;
    do {
        token = lexer_get_next_token();

        switch (token.type) {
            case TOKEN_IDENTIFIER:
                printf("%zu:IDENTIFIER:%s\n", token.line, token.data.str_val);
                token_destroy(&token);
                break;

            case TOKEN_KEYWORD_DEF:
            case TOKEN_KEYWORD_VAR:
            case TOKEN_KEYWORD_VAL:
            case TOKEN_KEYWORD_IF:
            case TOKEN_KEYWORD_ELSE:
            case TOKEN_KEYWORD_WHILE:
            case TOKEN_KEYWORD_RETURN:
                printf("%zu:%s\n", token.line, token_type_to_string(token.type));
                break;

            case TOKEN_LIT_INT:
                printf("%zu:LIT_INT:%lld\n", token.line, (long long)token.data.int_val);
                break;

            case TOKEN_LIT_FLOAT:
                printf("%zu:LIT_FLOAT:%g\n", token.line, token.data.float_val);
                break;

            case TOKEN_LIT_STRING:
                printf("%zu:LIT_STRING:%s\n", token.line, token.data.str_val);
                token_destroy(&token);
                break;

            default:
                printf("%zu:%s\n", token.line, token_type_to_string(token.type));
                break;
        }

    } while (token.type != TOKEN_EOF);

    return 0;
}
