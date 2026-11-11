/**
 * @file test_parser.c
 * @brief Testovací endpoint pro IFJ26 parser pro spouštění z Python runneru.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/parser/parser.h"
#include "../../src/_utils/error.h"

int main(int argc, char *argv[]) {
    bool dump_ast = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            fprintf(stderr, "Použití: %s [--ast] < vstup.scala\n", argv[0]);
            return 0;
        }
        if (strcmp(argv[i], "--ast") == 0) {
            dump_ast = true;
        }
    }

    // Spuštění syntaktické analýzy (čte stdin přes lexer)
    ParserStatus status = parse();

    // Vyhodnocení návratového stavu podle IFJ26:
    // RC = 1: lexikální chyba
    // RC = 2: syntaktická chyba
    // RC = 0: úspěch
    int rc = 0;
    if (status.flag == LEX_FAIL || status.error.type == ERR_LEXICAL) {
        rc = 1;
    } else if (status.flag == PAR_FAIL || status.error.type == ERR_SYNTAX) {
        rc = 2;
    } else if (ERROR_IS_ERR(status.error)) {
        rc = (int)status.error.type;
    } else if (dump_ast) {
        // Úspěch: volitelný výpis stromu jako S-výrazu pro porovnání s .ast
        ast_dump_sexpr(status.new_node, stdout);
        fputc('\n', stdout);
    }

    // Strom se uvolní na všech cestách; případný částečný strom z chybové cesty také
    ast_destroy(status.new_node);
    return rc;
}
