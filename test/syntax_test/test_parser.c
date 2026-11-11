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
    if (status.flag == LEX_FAIL || status.error.type == ERR_LEXICAL) {
        return 1;
    }

    if (status.flag == PAR_FAIL || status.error.type == ERR_SYNTAX) {
        return 2;
    }

    if (ERROR_IS_ERR(status.error)) {
        return (int)status.error.type;
    }

    // Success: optionally dump the tree as an S-expression for comparison with .ast
    if (dump_ast) {
        ast_dump_sexpr(status.new_node, stdout);
        fputc('\n', stdout);
    }
    ast_destroy(status.new_node);

    return 0;
}
