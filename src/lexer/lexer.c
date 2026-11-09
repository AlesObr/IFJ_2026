/**
 * @project IFJ26 Compiler
 * @file lexer.c
 * @brief Lexer module implementation for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @author Admiral (xkolekj00)
 * @note Standard: ISO C11
 */

#include "lexer.h"
#include "../_utils/dstring.h"
#include "../_utils/error.h"
#include "../_utils/token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*All Global vars. Below are the important ones to keep current state of Lexer */
static FILE *source_file = NULL; /** The input file being lexed - it will be de facto STDIN */
static size_t current_line = 1;	 /** The current line number in the source file being lexed */
static size_t current_col = 1;	 /** The current column number in the source file being lexed */
static size_t last_line_len = 0; /** The length of the previous line */

static size_t token_start_line = 1; /** Starting line of a token */
static size_t token_start_col = 1;	/** Starting column of a token */
static Stack indent_stack;
static size_t line_indent = 0;
static size_t pending_outdents = 0;
static bool at_line_start = true;
static size_t paren_depth = 0;
static size_t block_comment_depth = 0;
static int block_comment_previous = 0;
static bool indent_frozen = false; /** A comment ended indentation counting on this line */
static bool line_has_tab = false;  /** Tab in indentation, an error only if a token follows */
static bool pending_dot = false;   /** Dot after an integer that is not part of the literal */
static bool newline_before_token = false; /** LF inside parentheses before the current token */

/** Resets the state for the start of a new logical line. */
static void reset_line_state(void) {
	at_line_start = true;
	line_indent = 0;
	indent_frozen = false;
	line_has_tab = false;
}

bool lexer_newline_before_token(void) {
	return newline_before_token;
}

/** Character that would continue an operator in Scala (i.e. not two separate tokens). */
static bool continues_operator(int c) {
	return c == '=' || c == '>' || c == '<' || c == '@' || c == ':' || c == '/' || c == '+' ||
		   c == '-' || c == '*' || c == '|';
}

/** Creates an integer token; a non-zero literal must not start with zero. */
static Error make_int_token(Token *out_token, dstring_t *buffer) {
	const char *text = dstring_get(buffer);
	int64_t value = (int64_t)strtoll(text, NULL, 10);
	if (text[0] == '0' && value != 0) {
		return ERROR_EMIT(ERR_LEXICAL, current_line, current_col, "Leading zero in number.");
	}
	token_make_int(out_token, value, token_start_line, token_start_col);
	return ERROR_OK();
}

static size_t indent_top(void) {
	StackElement *element = stack_top(&indent_stack);
	return element != NULL ? *(size_t *)element->data : 0;
}

static void indent_push(size_t value) {
	size_t *stored = malloc(sizeof(*stored));
	if (stored == NULL)
		exit(EXIT_FAILURE);
	*stored = value;
	stack_push(&indent_stack, stored);
}

static void indent_pop(void) {
	void *element = stack_pop(&indent_stack);
	free(element);
}

void lexer_init(FILE *input) {
	source_file = input;
	while (indent_stack.top != NULL)
		indent_pop();
	current_line = 1;
	current_col = 0;
	last_line_len = 0;
	stack_init(&indent_stack);
	indent_push(0);
	line_indent = 0;
	pending_outdents = 0;
	at_line_start = true;
	paren_depth = 0;
	block_comment_depth = 0;
	block_comment_previous = 0;
	indent_frozen = false;
	line_has_tab = false;
	pending_dot = false;
	newline_before_token = false;
}

int lexer_take(LexemeState *lexeme, FILE *source_file) {
	lexeme->current_lexeme = getc(source_file);
	lexeme->previous_col = current_col;
	lexeme->previous_line = current_line;

	switch (lexeme->current_lexeme) {
		case '\n':
			current_line++;
			last_line_len = current_col;
			current_col = 0;

			// out_token -> token type = EOL
			return lexeme->current_lexeme;
		case EOF: //** Possible two entries - real EOF (feof()) and read error (ferror()) */
			return EOF;
		default:
			current_col++;
			return lexeme->current_lexeme;
	}
}

void lexer_give(LexemeState *lexeme, FILE *source_file) {
	if (lexeme->current_lexeme == EOF) {
		return;
	} // Guard against eof, which should NEVER get this far
	current_col = lexeme->previous_col;
	current_line = lexeme->previous_line;
	ungetc(lexeme->current_lexeme, source_file);
}

TokenType is_it_keyword(const char *str) {
	if (strcmp(str, "def") == 0) {
		return TOKEN_KEYWORD_DEF;
	} else if (strcmp(str, "do") == 0) {
		return TOKEN_KEYWORD_DO;
	} else if (strcmp(str, "class") == 0) {
		return TOKEN_KEYWORD_CLASS;
	} else if (strcmp(str, "case") == 0) {
		return TOKEN_KEYWORD_CASE;
	} else if (strcmp(str, "extension") == 0) {
		return TOKEN_KEYWORD_EXTENSION;
	} else if (strcmp(str, "type") == 0) {
		return TOKEN_KEYWORD_TYPE;
	} else if (strcmp(str, "match") == 0) {
		return TOKEN_KEYWORD_MATCH;
	} else if (strcmp(str, "Boolean") == 0) {
		return TOKEN_KEYWORD_BOOLEAN;
	} else if (strcmp(str, "List") == 0) {
		return TOKEN_KEYWORD_LIST;
	} else if (strcmp(str, "for") == 0) {
		return TOKEN_KEYWORD_FOR;
	} else if (strcmp(str, "Int") == 0) {
		return TOKEN_KEYWORD_INT;
	} else if (strcmp(str, "Double") == 0) {
		return TOKEN_KEYWORD_DOUBLE;
	} else if (strcmp(str, "String") == 0) {
		return TOKEN_KEYWORD_STRING;
	} else if (strcmp(str, "Unit") == 0) {
		return TOKEN_KEYWORD_UNIT;
	} else if (strcmp(str, "var") == 0) {
		return TOKEN_KEYWORD_VAR;
	} else if (strcmp(str, "val") == 0) {
		return TOKEN_KEYWORD_VAL;
	} else if (strcmp(str, "if") == 0) {
		return TOKEN_KEYWORD_IF;
	} else if (strcmp(str, "else") == 0) {
		return TOKEN_KEYWORD_ELSE;
	} else if (strcmp(str, "while") == 0) {
		return TOKEN_KEYWORD_WHILE;
	} else if (strcmp(str, "then") == 0) {
		return TOKEN_KEYWORD_THEN;
	} else if (strcmp(str, "import") == 0) {
		return TOKEN_KEYWORD_IMPORT;
	} else if (strcmp(str, "return") == 0) {
		return TOKEN_KEYWORD_RETURN;
	} else if (strcmp(str, "true") == 0) {
		return TOKEN_KEYWORD_TRUE;
	} else if (strcmp(str, "false") == 0) {
		return TOKEN_KEYWORD_FALSE;
	}
	return TOKEN_IDENTIFIER;
}

Error lexer_get_next_token(Token *out_token) {
	if (!out_token) {
		return ERROR_EMIT(ERR_INTERNAL, current_line, current_col, "Output token pointer is NULL.");
	}

	token_init(out_token);
	newline_before_token = false;

	if (pending_dot) {
		pending_dot = false;
		token_make_simple(out_token, TOKEN_DOT, current_line, current_col);
		return ERROR_OK();
	}

	LexerState current_state = STATE_START;
	dstring_t token_buffer;
	if (!dstring_init(&token_buffer)) {
		return ERROR_EMIT(
			ERR_INTERNAL, current_line, current_col, "Failed to initialize token buffer.");
	}

	for (;;) {
		LexemeState lexeme;
		lexeme.current_lexeme = lexer_take(&lexeme, source_file);

		switch (current_state) {
			case STATE_START:

				if (block_comment_depth > 0) {
					if (lexeme.current_lexeme == EOF) {
						dstring_free(&token_buffer);
						return ERROR_EMIT(
							ERR_LEXICAL, current_line, current_col, "Unterminated block comment.");
					}
					if (block_comment_previous == '/' && lexeme.current_lexeme == '*') {
						block_comment_depth++;
						block_comment_previous = 0;
						continue;
					}
					if (block_comment_previous == '*' && lexeme.current_lexeme == '/') {
						block_comment_depth--;
						block_comment_previous = 0;
						continue;
					}
					block_comment_previous = lexeme.current_lexeme;
					if (lexeme.current_lexeme == '\n') {
						if (paren_depth > 0) {
							newline_before_token = true;
							continue;
						}
						reset_line_state();
						token_make_simple(
							out_token, TOKEN_EOL, lexeme.previous_line, lexeme.previous_col + 1);
						dstring_free(&token_buffer);
						return ERROR_OK();
					}
					continue;
				}
				if (lexeme.current_lexeme == EOF && ferror(source_file) != 0) {
					dstring_free(&token_buffer);
					return ERROR_EMIT(
						ERR_INTERNAL, current_line, current_col, "Failed to read token from file.");
				} // @dev will probably need to be somehow incorporated into lexer_take
				if (lexeme.current_lexeme == EOF) {
					if (indent_stack.size > 1) {
						pending_outdents = indent_stack.size - 1;
						while (indent_stack.size > 1)
							indent_pop();
					}
					if (pending_outdents > 0) {
						pending_outdents--;
						token_make_simple(out_token, TOKEN_OUTDENT, current_line, 1);
						dstring_free(&token_buffer);
						return ERROR_OK();
					}
					token_make_simple(out_token, TOKEN_EOF, current_line, current_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (pending_outdents > 0) {
					lexer_give(&lexeme, source_file);
					pending_outdents--;
					token_make_simple(out_token, TOKEN_OUTDENT, current_line, 1);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == ' ') {
					if (at_line_start && !indent_frozen) {
						line_indent++;
					}
					continue;
				}
				if (lexeme.current_lexeme == '\t') {
					if (at_line_start && !indent_frozen) {
						line_has_tab = true;
					}
					continue;
				}
				if (lexeme.current_lexeme != '\n' && lexeme.current_lexeme != '/') {
					if (line_has_tab) {
						dstring_free(&token_buffer);
						return ERROR_EMIT(ERR_LEXICAL, current_line, current_col, "Tab found.\n");
					}
					if (at_line_start && current_line == 1 && line_indent < 4) {
						at_line_start = false;
					}
				}
				if (at_line_start && paren_depth == 0 && lexeme.current_lexeme != '\n' &&
					lexeme.current_lexeme != '/') {
					if (line_indent > indent_top()) {
						indent_push(line_indent);
						at_line_start = false;
						lexer_give(&lexeme, source_file);
						token_make_simple(out_token, TOKEN_INDENT, current_line, 1);
						dstring_free(&token_buffer);
						return ERROR_OK();
					}
					while (indent_stack.size > 1 && line_indent < indent_top()) {
						indent_pop();
						pending_outdents++;
					}
					if (line_indent != indent_top()) {
						dstring_free(&token_buffer);
						fprintf(stderr, "Line %zu, Col %zu: Inconsistent indentation.\n",
							current_line, current_col);
						return ERROR_EMIT(
							ERR_LEXICAL, current_line, current_col, "Inconsistent indentation.");
					}
					at_line_start = false;
					if (pending_outdents > 0) {
						lexer_give(&lexeme, source_file);
						pending_outdents--;
						token_make_simple(out_token, TOKEN_OUTDENT, current_line, 1);
						dstring_free(&token_buffer);
						return ERROR_OK();
					}
				}
				if (lexeme.current_lexeme == '\n') {
					if (paren_depth > 0) {
						newline_before_token = true;
						continue;
					}
					token_make_simple(
						out_token, TOKEN_EOL, lexeme.previous_line, lexeme.previous_col + 1);
					reset_line_state();
					dstring_free(&token_buffer);
					return ERROR_OK();
				} // @dev maybe return this only when it makes sense?
				if (lexeme.current_lexeme == '/') {
					LexemeState lookahead;
					int next = lexer_take(&lookahead, source_file);
					if (next == '/') {
						do {
							next = lexer_take(&lookahead, source_file);
						} while (next != '\n' && next != EOF);
						if (next == EOF) {
							continue; // EOF and pending OUTDENTs are handled at the start of the
									  // state
						}
						if (paren_depth > 0) {
							newline_before_token = true;
							continue;
						}
						token_make_simple(out_token, TOKEN_EOL, lookahead.previous_line,
							lookahead.previous_col + 1);
						reset_line_state();
						dstring_free(&token_buffer);
						return ERROR_OK();
					}
					if (next == '*') {
						block_comment_depth = 1;
						block_comment_previous = 0;
						indent_frozen = true;
						continue;
					}
					lexer_give(&lookahead, source_file);
					if (line_has_tab) {
						dstring_free(&token_buffer);
						return ERROR_EMIT(ERR_LEXICAL, current_line, current_col,
							"Invalid token at start of line.");
					}
					if (at_line_start && paren_depth == 0) {
						if (line_indent > indent_top()) {
							indent_push(line_indent);
							at_line_start = false;
							lexer_give(&lexeme, source_file);
							token_make_simple(out_token, TOKEN_INDENT, current_line, 1);
							dstring_free(&token_buffer);
							return ERROR_OK();
						}
						while (indent_stack.size > 1 && line_indent < indent_top()) {
							indent_pop();
							pending_outdents++;
						}
						if (line_indent != indent_top()) {
							dstring_free(&token_buffer);
							return ERROR_EMIT(ERR_LEXICAL, current_line, current_col,
								"Inconsistent indentation.");
						}
						at_line_start = false;
						if (pending_outdents > 0) {
							lexer_give(&lexeme, source_file);
							pending_outdents--;
							token_make_simple(out_token, TOKEN_OUTDENT, current_line, 1);
							dstring_free(&token_buffer);
							return ERROR_OK();
						}
					}
					at_line_start = false;
					token_make_simple(out_token, TOKEN_DIV, current_line, current_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == '0') {
					token_start_line = lexeme.previous_line;
					token_start_col = lexeme.previous_col;
					current_state = STATE_ZERO_SEEN;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				if (lexeme.current_lexeme >= '1' && lexeme.current_lexeme <= '9') {
					token_start_line = current_line;
					token_start_col = current_col;
					current_state = STATE_NUMBER_SEEN;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				if (lexeme.current_lexeme == '_') {
					token_start_line = current_line;
					token_start_col = current_col;
					current_state = STATE_DIV_DEC;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					lexeme.current_lexeme = lexer_take(&lexeme, source_file);
					if ((lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') ||
						lexeme.current_lexeme == '_') {
						dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
						current_state = STATE_CHAR;
						continue;
					}
					if ((lexeme.current_lexeme >= 'A' && lexeme.current_lexeme <= 'Z') ||
						(lexeme.current_lexeme >= 'a' && lexeme.current_lexeme <= 'z')) {
						token_start_line = current_line;
						token_start_col = current_col;
						dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
						current_state = STATE_CHAR;
						continue;
					}
					lexer_give(&lexeme, source_file);
					// @dev single '_' is really a valid input? what the f am I supposed to output
					token_make_simple(
						out_token, TOKEN_UNDERSCORE, token_start_line, token_start_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == '.') {
					token_start_line = current_line;
					token_start_col = current_col;
					current_state = STATE_WANNA_BE_DOUBLE;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					lexeme.current_lexeme = lexer_take(&lexeme, source_file);
					if (!(lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9')) {
						lexer_give(&lexeme, source_file);
						token_make_simple(out_token, TOKEN_DOT, current_line, current_col);
						dstring_free(&token_buffer);
						return ERROR_OK();
					}
					lexer_give(&lexeme, source_file);
					continue;
				}
				if (lexeme.current_lexeme == '(') {
					paren_depth++;
					token_make_simple(out_token, TOKEN_LPAREN, current_line, current_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == ')') {
					if (paren_depth > 0)
						paren_depth--;
					token_make_simple(out_token, TOKEN_RPAREN, current_line, current_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}

				if (lexeme.current_lexeme == '=') {
					token_start_line = current_line;
					token_start_col = current_col;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					lexeme.current_lexeme = lexer_take(&lexeme, source_file);
					if (lexeme.current_lexeme == '=') {
						dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
						lexeme.current_lexeme = lexer_take(&lexeme, source_file);
						if (continues_operator(lexeme.current_lexeme)) {
							lexer_give(&lexeme, source_file);
							dstring_free(&token_buffer);
							return ERROR_EMIT(
								ERR_LEXICAL, current_line, current_col, "Invalid operand.\n");
						}
						lexer_give(&lexeme, source_file);
						token_make_str(
							out_token, TOKEN_EQ, &token_buffer, token_start_line, token_start_col);
						dstring_free(&token_buffer);
						return ERROR_OK();
					}
					lexer_give(&lexeme, source_file);
					token_make_simple(out_token, TOKEN_ASSIGN, token_start_line, token_start_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}

				if (lexeme.current_lexeme == ':') {
					token_make_simple(out_token, TOKEN_COLON, current_line, current_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == '@') {
					token_start_line = lexeme.previous_line;
					token_start_col = lexeme.previous_col;
					static const char main_name[] = "main";
					for (size_t i = 0; i < sizeof(main_name) - 1; i++) {
						lexeme.current_lexeme = lexer_take(&lexeme, source_file);
						if (lexeme.current_lexeme != main_name[i]) {
							dstring_free(&token_buffer);
							return ERROR_EMIT(
								ERR_LEXICAL, current_line, current_col, "Expected @main.");
						}
					}
					lexeme.current_lexeme = lexer_take(&lexeme, source_file);
					if ((lexeme.current_lexeme >= 'A' && lexeme.current_lexeme <= 'Z') ||
						(lexeme.current_lexeme >= 'a' && lexeme.current_lexeme <= 'z') ||
						(lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') ||
						lexeme.current_lexeme == '_') {
						dstring_free(&token_buffer);
						return ERROR_EMIT(
							ERR_LEXICAL, current_line, current_col, "Invalid @main token.");
					}
					lexer_give(&lexeme, source_file);
					token_make_simple(out_token, TOKEN_AT_MAIN, token_start_line, token_start_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == ',') {
					token_make_simple(out_token, TOKEN_COMMA, current_line, current_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == '/') {
					token_make_simple(out_token, TOKEN_DIV, current_line, current_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}

				if (lexeme.current_lexeme == '>') {
					token_start_line = current_line;
					token_start_col = current_col;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					lexeme.current_lexeme = lexer_take(&lexeme, source_file);
					if (lexeme.current_lexeme == '=') {
						dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
						lexeme.current_lexeme = lexer_take(&lexeme, source_file);
						if (continues_operator(lexeme.current_lexeme)) {
							lexer_give(&lexeme, source_file);
							dstring_free(&token_buffer);
							return ERROR_EMIT(
								ERR_LEXICAL, current_line, current_col, "Invalid operand.\n");
						}
						lexer_give(&lexeme, source_file);
						token_make_str(
							out_token, TOKEN_GTE, &token_buffer, token_start_line, token_start_col);
						dstring_free(&token_buffer);
						return ERROR_OK();
					}
					lexer_give(&lexeme, source_file);
					token_make_simple(out_token, TOKEN_GT, token_start_line, token_start_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == '<') {
					token_start_line = current_line;
					token_start_col = current_col;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					lexeme.current_lexeme = lexer_take(&lexeme, source_file);
					if (lexeme.current_lexeme == '=') {
						dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
						lexeme.current_lexeme = lexer_take(&lexeme, source_file);
						if (continues_operator(lexeme.current_lexeme)) {
							lexer_give(&lexeme, source_file);
							dstring_free(&token_buffer);
							return ERROR_EMIT(
								ERR_LEXICAL, current_line, current_col, "Invalid operand.\n");
						}
						lexer_give(&lexeme, source_file);
						token_make_str(
							out_token, TOKEN_LTE, &token_buffer, token_start_line, token_start_col);
						dstring_free(&token_buffer);
						return ERROR_OK();
					}
					lexer_give(&lexeme, source_file);
					token_make_simple(out_token, TOKEN_LT, token_start_line, token_start_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == '-') {
					token_make_simple(out_token, TOKEN_MINUS, current_line, current_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == '*') {
					token_make_simple(out_token, TOKEN_MUL, current_line, current_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == '|') {
					token_start_line = current_line;
					token_start_col = current_col;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					lexeme.current_lexeme = lexer_take(&lexeme, source_file);
					if (lexeme.current_lexeme == '|') {
						dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
						lexeme.current_lexeme = lexer_take(&lexeme, source_file);
						if (continues_operator(lexeme.current_lexeme)) {
							lexer_give(&lexeme, source_file);
							dstring_free(&token_buffer);
							return ERROR_EMIT(
								ERR_LEXICAL, current_line, current_col, "Invalid operand.\n");
						}
						lexer_give(&lexeme, source_file);
						token_make_str(
							out_token, TOKEN_OR, &token_buffer, token_start_line, token_start_col);
						dstring_free(&token_buffer);
						return ERROR_OK();
					}
					lexer_give(&lexeme, source_file);
					token_make_simple(out_token, TOKEN_PIPE, token_start_line, token_start_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == '+') {
					token_make_simple(out_token, TOKEN_PLUS, current_line, current_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == ';') {
					token_make_simple(out_token, TOKEN_SEMICOLON, current_line, current_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				if (lexeme.current_lexeme == '!') {
					token_start_line = current_line;
					token_start_col = current_col;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					lexeme.current_lexeme = lexer_take(&lexeme, source_file);
					if (lexeme.current_lexeme == '=') {
						dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
						lexeme.current_lexeme = lexer_take(&lexeme, source_file);
						if (continues_operator(lexeme.current_lexeme)) {
							lexer_give(&lexeme, source_file);
							dstring_free(&token_buffer);
							return ERROR_EMIT(
								ERR_LEXICAL, current_line, current_col, "Invalid operand.\n");
						}
						lexer_give(&lexeme, source_file);
						token_make_str(
							out_token, TOKEN_NEQ, &token_buffer, token_start_line, token_start_col);
						dstring_free(&token_buffer);
						return ERROR_OK();
					}
					lexer_give(&lexeme, source_file);
					dstring_free(&token_buffer);
					return ERROR_EMIT(ERR_LEXICAL, current_line, current_col, "Invalid operand.\n");
				}
				if ((lexeme.current_lexeme >= 'A' && lexeme.current_lexeme <= 'Z') ||
					(lexeme.current_lexeme >= 'a' && lexeme.current_lexeme <= 'z')) {
					token_start_line = lexeme.previous_line;
					token_start_col = lexeme.previous_col;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					current_state = STATE_CHAR;
					continue;
				}
				if (lexeme.current_lexeme == '"') {
					token_start_line = lexeme.previous_line;
					token_start_col = lexeme.previous_col;
					current_state = STATE_STRING;
					lexeme.current_lexeme = lexer_take(&lexeme, source_file);
					if (lexeme.current_lexeme == '"') {
						lexeme.current_lexeme = lexer_take(&lexeme, source_file);
						if (lexeme.current_lexeme == '"') {
							current_state = STATE_ML_STRING;
							continue;
						}
						lexer_give(&lexeme, source_file);
						token_make_str(out_token, TOKEN_LIT_STRING, &token_buffer, token_start_line,
							token_start_col);
						dstring_free(&token_buffer);
						return ERROR_OK();
					}
					lexer_give(&lexeme, source_file);
					continue;
				}
				dstring_free(&token_buffer);
				return ERROR_EMIT(ERR_LEXICAL, current_line, current_col, "Unexpected character.");

			case STATE_NUMBER_SEEN:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				if (lexeme.current_lexeme == '.') {
					LexemeState after_dot;
					int next_char = lexer_take(&after_dot, source_file);
					lexer_give(&after_dot, source_file);
					if (next_char >= '0' && next_char <= '9') {
						current_state = STATE_WANNA_BE_DOUBLE;
						dstring_append_char(&token_buffer, '.');
						continue;
					}
					pending_dot = true; // 5.toString: an integer followed by a member
					Error int_err = make_int_token(out_token, &token_buffer);
					dstring_free(&token_buffer);
					return int_err;
				}
				if (lexeme.current_lexeme == 'e' || lexeme.current_lexeme == 'E') {
					current_state = STATE_WANNA_BE_EXP_1;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				if (lexeme.current_lexeme == '_') {
					current_state = STATE_DIV_DEC;
					continue;
				}

				// Return the integer token if we reach a non-digit character
				lexer_give(&lexeme, source_file);
				Error number_err = make_int_token(out_token, &token_buffer);
				dstring_free(&token_buffer);
				return number_err;
			case STATE_ZERO_SEEN:
				if (lexeme.current_lexeme == '.') {
					LexemeState after_dot;
					int next_char = lexer_take(&after_dot, source_file);
					lexer_give(&after_dot, source_file);
					if (next_char >= '0' && next_char <= '9') {
						current_state = STATE_WANNA_BE_DOUBLE;
						dstring_append_char(&token_buffer, '.');
						continue;
					}
					pending_dot = true; // 5.toString: an integer followed by a member
					Error int_err = make_int_token(out_token, &token_buffer);
					dstring_free(&token_buffer);
					return int_err;
				}
				if (lexeme.current_lexeme == 'e' || lexeme.current_lexeme == 'E') {
					current_state = STATE_WANNA_BE_EXP_1;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				if (lexeme.current_lexeme == '_') {
					current_state = STATE_DIV_DEC;
					continue;
				}

				lexer_give(&lexeme, source_file);
				token_make_int(
					out_token, 0, token_start_line, token_start_col); // lexeme.current_lexeme ? 0
				dstring_free(&token_buffer);
				return ERROR_OK();

				continue;
			case STATE_WANNA_BE_DOUBLE:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					current_state = STATE_DOUBLE;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				dstring_free(&token_buffer);
				return ERROR_EMIT(ERR_LEXICAL, current_line, current_col, "Malformed number.");
			case STATE_DOUBLE:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				if (lexeme.current_lexeme == '_') {
					// dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					current_state = STATE_UNDER_SCORE;
					continue;
				}

				// Exponent
				if (lexeme.current_lexeme == 'e' || lexeme.current_lexeme == 'E') {
					current_state = STATE_WANNA_BE_EXP_2;
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}

				if (lexeme.current_lexeme == 'd' || lexeme.current_lexeme == 'D') {
					current_state = STATE_SUFFIX;
					// We don't add the suffix to the buffer, as strtod will ignore it and we want
					// to keep the value clean
					continue;
				}

				lexer_give(&lexeme, source_file);
				double double_val = strtod(dstring_get(&token_buffer), NULL);
				token_make_float(out_token, double_val, token_start_line, token_start_col);
				dstring_free(&token_buffer);
				return ERROR_OK();
				continue;
			case STATE_DIV_DEC:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					current_state = STATE_NUMBER_SEEN;
					continue;
				}
				if (lexeme.current_lexeme == '_') {
					continue;
				}
				dstring_free(&token_buffer);
				return ERROR_EMIT(
					ERR_LEXICAL, current_line, current_col, "Number cannot end with underscore.");
			case STATE_UNDER_SCORE:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					current_state = STATE_DOUBLE;
					continue;
				}
				if (lexeme.current_lexeme == '_') {
					continue;
				}
				dstring_free(&token_buffer);
				return ERROR_EMIT(
					ERR_LEXICAL, current_line, current_col, "Malformed number followed by EOF.");
				continue;
			case STATE_WANNA_BE_EXP_1:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					current_state = STATE_EXPONENT;
					continue;
				}
				if (lexeme.current_lexeme == '+' || lexeme.current_lexeme == '-') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					current_state = STATE_EXP_SIGN_1;
					continue;
				}
				dstring_free(&token_buffer);
				return ERROR_EMIT(
					ERR_LEXICAL, current_line, current_col, "Malformed number followed by EOF.");
				continue;
			case STATE_WANNA_BE_EXP_2:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					current_state = STATE_EXPONENT;
					continue;
				}
				if (lexeme.current_lexeme == '+' || lexeme.current_lexeme == '-') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					current_state = STATE_EXP_SIGN_2;
					continue;
				}
				dstring_free(&token_buffer);
				return ERROR_EMIT(
					ERR_LEXICAL, current_line, current_col, "Malformed number followed by EOF.");
				continue; // @dev does it need to be here?
			case STATE_EXP_SIGN_1:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					current_state = STATE_EXPONENT;
					continue;
				}
				dstring_free(&token_buffer);
				return ERROR_EMIT(
					ERR_LEXICAL, current_line, current_col, "Malformed number followed by EOF.");
				continue;
			case STATE_EXP_SIGN_2:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					current_state = STATE_EXPONENT;
					continue;
				}
				dstring_free(&token_buffer);
				return ERROR_EMIT(
					ERR_LEXICAL, current_line, current_col, "Malformed number followed by EOF.");
				continue;
			case STATE_EXP_UNDER_SCORE:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					current_state = STATE_EXPONENT;
					continue;
				}
				if (lexeme.current_lexeme == '_') {
					continue;
				}
				dstring_free(&token_buffer);
				return ERROR_EMIT(
					ERR_LEXICAL, current_line, current_col, "Number cannot end with underscore.");
			case STATE_EXPONENT:
				if (lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
					// @dev exponent je koncový stav dka
				}
				if (lexeme.current_lexeme == '_') {
					current_state = STATE_EXP_UNDER_SCORE;
					continue;
				}
				if (lexeme.current_lexeme == 'd' || lexeme.current_lexeme == 'D') {
					current_state = STATE_SUFFIX;
					// We don't add the suffix to the buffer, as strtod will ignore it and we want
					// to keep the value clean
					continue;
				}
				lexer_give(&lexeme, source_file);
				double_val = strtod(dstring_get(&token_buffer), NULL);
				token_make_float(out_token, double_val, token_start_line,
					token_start_col); // not working good at all
				dstring_free(&token_buffer);
				return ERROR_OK();
				continue;
			case STATE_SUFFIX:
				lexer_give(&lexeme, source_file);
				token_make_float(out_token, strtod(dstring_get(&token_buffer), NULL),
					token_start_line, token_start_col);
				dstring_free(&token_buffer);
				return ERROR_OK();
				continue;
			case STATE_CHAR:
				if ((lexeme.current_lexeme >= 'A' && lexeme.current_lexeme <= 'Z') ||
					(lexeme.current_lexeme >= 'a' && lexeme.current_lexeme <= 'z') ||
					(lexeme.current_lexeme >= '0' && lexeme.current_lexeme <= '9') ||
					lexeme.current_lexeme == '_') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				lexer_give(&lexeme, source_file);
				if (is_it_keyword(token_buffer.data) == TOKEN_IDENTIFIER) {
					token_make_str(out_token, TOKEN_IDENTIFIER, &token_buffer, token_start_line,
						token_start_col);
				} else {
					token_make_simple(out_token, is_it_keyword(token_buffer.data), token_start_line,
						token_start_col);
				}
				dstring_free(&token_buffer);
				return ERROR_OK();
				continue;
			case STATE_STRING:
				if (lexeme.current_lexeme == EOF || lexeme.current_lexeme == '\n') {
					dstring_free(&token_buffer);
					return ERROR_EMIT(
						ERR_LEXICAL, current_line, current_col, "Unterminated string.");
				}
				if (lexeme.current_lexeme == '\\') {
					int escaped = lexer_take(&lexeme, source_file);
					char value;
					switch (escaped) {
						case 'n':
							value = '\n';
							break;
						case 't':
							value = '\t';
							break;
						case 'r':
							value = '\r';
							break;
						case '"':
							value = '"';
							break;
						case '\\':
							value = '\\';
							break;
						default:
							dstring_free(&token_buffer);
							return ERROR_EMIT(
								ERR_LEXICAL, current_line, current_col, "Invalid string escape.");
					}
					dstring_append_char(&token_buffer, value);
					continue;
				}
				if (lexeme.current_lexeme == '"') {
					token_make_str(out_token, TOKEN_LIT_STRING, &token_buffer, token_start_line,
						token_start_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
				continue;
				continue;
			case STATE_ML_STRING: // @dev this is faulty
				if (lexeme.current_lexeme == EOF) {
					dstring_free(&token_buffer);
					return ERROR_EMIT(
						ERR_LEXICAL, current_line, current_col, "Unterminated multiline string.");
				}
				if (lexeme.current_lexeme != '"') {
					dstring_append_char(&token_buffer, (char)lexeme.current_lexeme);
					continue;
				}
				size_t quotes = 1;
				while ((lexeme.current_lexeme = lexer_take(&lexeme, source_file)) == '"') {
					quotes++;
				}
				lexer_give(&lexeme, source_file);
				// The last three quotes end the literal, any preceding ones belong to it
				for (size_t i = (quotes >= 3 ? 3 : 0); i < quotes; i++) {
					dstring_append_char(&token_buffer, '"');
				}
				if (quotes >= 3) {
					token_make_str(out_token, TOKEN_LIT_STRING, &token_buffer, token_start_line,
						token_start_col);
					dstring_free(&token_buffer);
					return ERROR_OK();
				}
				continue;
			default:
				dstring_free(&token_buffer);
				return ERROR_EMIT(ERR_LEXICAL, current_line, current_col, "DKA ERROR");
				break;

				// @dev(Jirka Kolek) Implementace stavu pro chybu
		}
	}
	return ERROR_OK();
}
