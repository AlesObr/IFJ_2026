/**
 * @file test_codegen.c
 * @brief Harness generatoru kodu: parser -> semantika -> gen_run.
 *
 * Cte program IFJ26 ze stdin a na stdout vypise IFJcode26. Navratovy kod je kod chyby dle
 * zadani (1/2 lexer/parser, 3-10 semantika, 99 interni chyba vc. selhani generatoru), jinak 0.
 * Zprava chyby jde na stderr ve tvaru "[faze] radek:sloupec zprava".
 */

#include <stdio.h>

#include "../../src/_utils/error.h"
#include "../../src/code_gen/code_gen.h"
#include "../../src/parser/parser.h"
#include "../../src/semantic_analyzer/semantic.h"

static int report(const char *phase, Error err) {
	fprintf(stderr, "[%s] %zu:%zu %s\n", phase, err.line, err.col, err.message ? err.message : "");
	return (int)err.type;
}

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
		Error err = sem_run(status.new_node);
		if (ERROR_IS_ERR(err)) {
			rc = report("semantic", err);
		} else {
			err = gen_run(status.new_node);
			if (ERROR_IS_ERR(err)) {
				rc = report("code_gen", err);
			}
		}
	}
	fflush(stdout);
	ast_destroy(status.new_node);
	return rc;
}
