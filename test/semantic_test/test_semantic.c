/**
 * @file test_semantic.c
 * @brief Harness semanticke analyzy: parser -> sem_run.
 *
 * Cte program IFJ26 ze stdin. Navratovy kod je kod chyby dle zadani (1/2 lexer/parser,
 * 3-10 semantika, 99 interni chyba), 0 pri uspechu. Zprava semanticke chyby jde na stderr
 * ve tvaru "[semantic] radek:sloupec zprava" (posledni radek vystupu, vyuziva ho runner).
 */

#include <stdio.h>

#include "../../src/_utils/error.h"
#include "../../src/parser/parser.h"
#include "../../src/semantic_analyzer/semantic.h"

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
			fprintf(stderr, "[semantic] %zu:%zu %s\n", err.line, err.col,
				err.message ? err.message : "");
			rc = (int)err.type;
		}
	}
	ast_destroy(status.new_node);
	return rc;
}
