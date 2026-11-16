/**
 * @file test_codegen.c
 * @brief Harness generatoru kodu: parser -> code_gen bez semanticke analyzy.
 *
 * Cte program IFJ26 ze stdin a na stdout vypise IFJcode26. Navratovy kod: 1/2 pri chybe
 * lexeru/parseru, kod chyby generatoru (99), jinak 0. Semantika se nespousti, testovane
 * programy proto musi byt semanticky spravne.
 */

#include <stdio.h>

#include "../../src/_utils/error.h"
#include "../../src/code_gen/code_gen.h"
#include "../../src/parser/parser.h"

int main(void) {
	ParserStatus status = parse();
	int rc = 0;
	if (status.flag == LEX_FAIL || status.error.type == ERR_LEXICAL) {
		rc = 1;
	} else if (status.flag == PAR_FAIL || status.error.type == ERR_SYNTAX) {
		rc = 2;
	} else if (ERROR_IS_ERR(status.error)) {
		rc = (int)status.error.type;
	} else {
		Error err = code_gen(status.new_node);
		if (ERROR_IS_ERR(err)) {
			fprintf(stderr, "[code_gen] %zu:%zu %s\n", err.line, err.col,
				err.message ? err.message : "");
			rc = (int)err.type;
		}
	}
	ast_destroy(status.new_node);
	return rc;
}
