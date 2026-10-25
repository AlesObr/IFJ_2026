/**
 * @project IFJ26 Compiler
 * @file main.c
 * @brief Compiler entry point and pipeline orchestration.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#include "./_utils/error.h"
#include "./_utils/token.h"
#include "./lexer/lexer.h"
#include "./parser/parser.h"

int main(int argc, char *argv[]) {
	FILE *input_file = stdin; // Default to standard input

	if (argc > 1) {
		input_file = fopen(argv[1], "r");
		if (!input_file) {
			return ERR_INTERNAL;
		}
	}

	lexer_init(input_file);
	Token token;
	Error err;

	if (input_file != stdin) {
		fclose(input_file);
	}

	return ERR_OK;
}
