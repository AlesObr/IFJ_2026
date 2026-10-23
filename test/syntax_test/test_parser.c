/**
 * @file parser_ast_test.c
 * @brief Samostatný testovací endpoint pro parser a validaci AST.
 * Soubor byl vygenerován nástrojem generativní umělé inteligence (Gemini 3.8 flash)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "parser/parser.h"
#include "parser/ast.h"
#include "lexer/lexer.h"

// Reference na kořen vygenerovaného AST
// (přizpůsobte podle toho, zda je v parseru extern proměnná nebo getter)
extern ASTNode *g_ast_root;

int main(int argc, char *argv[]) {
    bool dump_ast = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--ast") == 0 || strcmp(argv[i], "-a") == 0) {
            dump_ast = true;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            fprintf(stderr, "Použití: %s [--ast|-a] < soubor.scala\n", argv[0]);
            return 0;
        }
    }

    // Spuštění syntaktické analýzy
    parse();

    /*
     * Vyhodnocení návratového stavu podle specifikace IFJ26:
     * RC = 0: syntakticky i lexikálně v pořádku
     * RC = 1: lexikální chyba
     * RC = 2: syntaktická chyba
     */

    // Pokud došlo k selhání, ukliďte případně částečné AST a vraťte chybový kód:
    // Příklad pro vaše rozhraní z parser.h:
    /*
    extern ParserStatus g_parser_status;
    if (g_parser_status.flag == LEX_FAIL) {
        if (g_ast_root) { ast_destroy(g_ast_root); g_ast_root = NULL; }
        return 1;
    }
    if (g_parser_status.flag == PAR_FAIL) {
        if (g_ast_root) { ast_destroy(g_ast_root); g_ast_root = NULL; }
        return 2;
    }
    */

    // Pokud byl předán přepínač --ast a máme sestavený strom
    if (dump_ast && g_ast_root != NULL) {
        ast_dump_sexpr(g_ast_root, stdout);
        printf("\n");
    }

    // Úklid paměti přes ast_destroy
    if (g_ast_root != NULL) {
        ast_destroy(g_ast_root);
        g_ast_root = NULL;
    }

    return 0;
}
