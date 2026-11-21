/**
 * @project IFJ26 Compiler
 * @file main.c
 * @brief Compiler entry point and pipeline orchestration.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 *
 * Pipeline: parser (drives the lexer) -> semantic analysis -> code generation. The program is
 * read from the standard input (or from the file given as the first argument), the IFJcode26
 * is written to the standard output and the exit code is the error code of the first failing
 * phase as defined by the specification (0 on success).
 */

#include "./_utils/error.h"
#include "./code_gen/code_gen.h"
#include "./parser/ast.h"
#include "./parser/parser.h"
#include "./semantic_analyzer/semantic.h"
#include <stdio.h>

/**
 * @brief Converts the parser result to the exit code of the compiler.
 * @param status Result of parse()
 * @return ERR_OK if the parser built the AST, the error code otherwise
 */
static int parser_exit_code(const ParserStatus *status) {
	if (status->flag == LEX_FAIL || status->error.type == ERR_LEXICAL) {
		return ERR_LEXICAL;
	}
	if (status->flag == PAR_FAIL || status->error.type == ERR_SYNTAX) {
		return ERR_SYNTAX;
	}
	if (status->flag == AST_FAIL) {
		return ERR_INTERNAL;
	}
	if (ERROR_IS_ERR(status->error)) {
		return (int)status->error.type;
	}
	return status->new_node != NULL ? ERR_OK : ERR_INTERNAL;
}

static void report(const char *phase, Error err) {
	fprintf(stderr, "[%s] %zu:%zu %s\n", phase, err.line, err.col, err.message ? err.message : "");
}

int main(int argc, char *argv[]) {
	// The parser reads the standard input, a file argument is redirected into it
	if (argc > 1 && freopen(argv[1], "r", stdin) == NULL) {
		return ERR_INTERNAL;
	}

	ParserStatus status = parse();
	int rc = parser_exit_code(&status);
	if (rc == ERR_OK) {
		Error err = sem_run(status.new_node);
		if (ERROR_IS_ERR(err)) {
			report("semantic", err);
			rc = (int)err.type;
		}
	}
	if (rc == ERR_OK) {
		Error err = gen_run(status.new_node);
		if (ERROR_IS_ERR(err)) {
			report("code_gen", err);
			rc = (int)err.type;
		}
	}

	fflush(stdout);
	ast_destroy(status.new_node);
	return rc;
}
