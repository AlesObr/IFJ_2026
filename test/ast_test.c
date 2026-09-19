/**
 * @file ast_test.c
 * @brief Unit tests for AST construction, S-expression dump, cleanup
 *        and out-of-memory (fault injection) behaviour.
 *
 * @note Build with allocator wrapping so the OOM tests work:
 *   gcc -std=c11 -g -fsanitize=address,undefined \
 *       -Wl,--wrap=malloc,--wrap=realloc,--wrap=calloc,--wrap=free \
 *       ast_test.c ../src/parser/ast.c ../src/_utils/dstring.c -o ast_test
 *
 * Without --wrap the file still compiles and runs; the OOM suite then
 * reports itself as skipped.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "../src/parser/ast.h"
#include "../src/_utils/dstring.h"

/* ------------------------------------------------------------------ */
/* Allocator wrappers: fault injection + live-allocation accounting.  */
/* ------------------------------------------------------------------ */

/* Enabled only by the dedicated OOM target, which links with -Wl,--wrap=... */
#ifndef AST_TEST_HAVE_WRAP
#define AST_TEST_HAVE_WRAP 0
#endif

#if AST_TEST_HAVE_WRAP

void *__real_malloc(size_t size);
void *__real_calloc(size_t n, size_t size);
void *__real_realloc(void *ptr, size_t size);
void __real_free(void *ptr);

/* Number of further allocations to let through; -1 disables injection. */
static long g_fail_countdown = -1;
/* Number of live blocks currently handed out through the wrappers. */
static long g_live_blocks = 0;
/* Set once injection actually fired, so the driver knows it ran out. */
static bool g_injection_fired = false;

static bool should_fail(void) {
	if (g_fail_countdown < 0) {
		return false;
	}
	if (g_fail_countdown == 0) {
		g_injection_fired = true;
		return true;
	}
	g_fail_countdown--;
	return false;
}

void *__wrap_malloc(size_t size) {
	if (should_fail()) {
		return NULL;
	}
	void *p = __real_malloc(size);
	if (p) {
		g_live_blocks++;
	}
	return p;
}

void *__wrap_calloc(size_t n, size_t size) {
	if (should_fail()) {
		return NULL;
	}
	void *p = __real_calloc(n, size);
	if (p) {
		g_live_blocks++;
	}
	return p;
}

void *__wrap_realloc(void *ptr, size_t size) {
	if (should_fail()) {
		return NULL; /* original block must stay valid -- that is the point */
	}
	void *p = __real_realloc(ptr, size);
	if (p && ptr == NULL) {
		g_live_blocks++; /* realloc(NULL, n) behaves like malloc */
	}
	return p;
}

void __wrap_free(void *ptr) {
	if (ptr) {
		g_live_blocks--;
	}
	__real_free(ptr);
}

#endif /* AST_TEST_HAVE_WRAP */

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

static char *dump_to_string(const ASTNode *node) {
	char *bp = NULL;
	size_t size = 0;
	FILE *stream = open_memstream(&bp, &size);
	assert(stream != NULL);

	ast_dump_sexpr(node, stream);
	fclose(stream);

	return bp;
}

static void make_dstring(dstring_t *s, const char *text) {
	bool ok = dstring_init(s);
	assert(ok);
	(void)ok;
	dstring_append_str(s, text);
}

/* ------------------------------------------------------------------ */
/* Functional tests                                                    */
/* ------------------------------------------------------------------ */

static void test_literals_and_binop(void) {
	printf("Running test_literals_and_binop... ");

	/* 42 + 3.14 */
	ASTNode *lit_int = ast_create_int(42, 1, 1);
	ASTNode *lit_float = ast_create_float(3.14, 1, 6);
	ASTNode *binop = ast_create_binop(OP_ADD, lit_int, lit_float, 1, 4);

	char *out = dump_to_string(binop);
	assert(strcmp(out, "(+ 42 3.14)") == 0);
	free(out);

	ast_destroy(binop);
	printf("PASSED\n");
}

static void test_unary_and_string_literal(void) {
	printf("Running test_unary_and_string_literal... ");

	dstring_t text;
	make_dstring(&text, "ahoj");

	ASTNode *str = ast_create_string(&text, 1, 1);
	char *out_str = dump_to_string(str);
	assert(strcmp(out_str, "\"ahoj\"") == 0);
	free(out_str);
	ast_destroy(str);

	/* !(-5) */
	ASTNode *neg = ast_create_unop(OP_UNARY_MINUS, ast_create_int(5, 1, 2), 1, 1);
	ASTNode *notted = ast_create_unop(OP_NOT, neg, 1, 1);
	char *out_un = dump_to_string(notted);
	assert(strcmp(out_un, "(! (- 5))") == 0);
	free(out_un);
	ast_destroy(notted);

	dstring_free(&text);
	printf("PASSED\n");
}

static void test_var_decl_and_assignment(void) {
	printf("Running test_var_decl_and_assignment... ");

	dstring_t var_name, type_name;
	make_dstring(&var_name, "x");
	make_dstring(&type_name, "i32");

	/* var x:i32 = 10 */
	ASTNode *init_val = ast_create_int(10, 1, 15);
	ASTNode *decl = ast_create_var_decl(&var_name, &type_name, false, init_val, 1, 1);

	char *out_decl = dump_to_string(decl);
	assert(strcmp(out_decl, "(var x:i32 10)") == 0);
	free(out_decl);
	ast_destroy(decl);

	/* const x = 10  (type inferred, so no colon in the dump) */
	ASTNode *decl_inferred =
		ast_create_var_decl(&var_name, NULL, true, ast_create_int(10, 1, 11), 1, 1);
	char *out_inferred = dump_to_string(decl_inferred);
	assert(strcmp(out_inferred, "(const x 10)") == 0);
	free(out_inferred);
	ast_destroy(decl_inferred);

	/* x = x + 1 */
	ASTNode *ident_left = ast_create_ident(&var_name, 2, 5);
	ASTNode *lit_one = ast_create_int(1, 2, 9);
	ASTNode *add_expr = ast_create_binop(OP_ADD, ident_left, lit_one, 2, 7);
	ASTNode *assign = ast_create_assignment(&var_name, add_expr, 2, 1);

	char *out_assign = dump_to_string(assign);
	assert(strcmp(out_assign, "(= x (+ x 1))") == 0);
	free(out_assign);
	ast_destroy(assign);

	dstring_free(&var_name);
	dstring_free(&type_name);
	printf("PASSED\n");
}

static void test_complex_block_and_control_flow(void) {
	printf("Running test_complex_block_and_control_flow... ");

	dstring_t fn_name, arg_name;
	make_dstring(&fn_name, "print");
	make_dstring(&arg_name, "val");

	ASTNode *block = ast_create_block(1, 1);

	ASTNode *cond = ast_create_binop(OP_GT,
	                                 ast_create_ident(&arg_name, 2, 9),
	                                 ast_create_int(0, 2, 15), 2, 13);

	ASTNode *call = ast_create_call(&fn_name, 2, 18);
	assert(ast_call_add_arg(call, ast_create_ident(&arg_name, 2, 24)));

	assert(ast_block_add_stmt(block, ast_create_if(cond, call, NULL, 2, 5)));
	assert(ast_block_add_stmt(block, ast_create_return(ast_create_null(3, 12), 3, 5)));

	char *out = dump_to_string(block);
	const char *expected = "(block (if (> val 0) (call print val)) (return null))";
	assert(strcmp(out, expected) == 0);

	free(out);
	ast_destroy(block);

	dstring_free(&fn_name);
	dstring_free(&arg_name);
	printf("PASSED\n");
}

/* Forces the node list past its initial capacity of 4, exercising realloc. */
static void test_nodelist_growth(void) {
	printf("Running test_nodelist_growth... ");

	dstring_t fn_name;
	make_dstring(&fn_name, "f");

	ASTNode *call = ast_create_call(&fn_name, 1, 1);
	assert(call != NULL);
	for (long i = 0; i < 17; ++i) {
		assert(ast_call_add_arg(call, ast_create_int(i, 1, 1)));
	}

	ASTNode *block = ast_create_block(1, 1);
	assert(block != NULL);
	for (int i = 0; i < 17; ++i) {
		assert(ast_block_add_stmt(block, ast_create_return(NULL, 1, 1)));
	}
	assert(ast_block_add_stmt(block, call));

	ast_destroy(block);
	dstring_free(&fn_name);
	printf("PASSED\n");
}

static void test_while_and_if_else(void) {
	printf("Running test_while_and_if_else... ");

	dstring_t name;
	make_dstring(&name, "i");

	/* while (i < 10) { i = i + 1 } */
	ASTNode *body = ast_create_block(1, 1);
	assert(ast_block_add_stmt(body,
		ast_create_assignment(&name,
			ast_create_binop(OP_ADD, ast_create_ident(&name, 1, 1),
			                 ast_create_int(1, 1, 1), 1, 1), 1, 1)));

	ASTNode *loop = ast_create_while(
		ast_create_binop(OP_LT, ast_create_ident(&name, 1, 1),
		                 ast_create_int(10, 1, 1), 1, 1),
		body, 1, 1);

	char *out = dump_to_string(loop);
	assert(strcmp(out, "(while (< i 10) (block (= i (+ i 1))))") == 0);
	free(out);
	ast_destroy(loop);

	/* if (i) return i; else return null; */
	ASTNode *branch = ast_create_if(ast_create_ident(&name, 1, 1),
	                                ast_create_return(ast_create_ident(&name, 1, 1), 1, 1),
	                                ast_create_return(ast_create_null(1, 1), 1, 1), 1, 1);
	char *out_if = dump_to_string(branch);
	assert(strcmp(out_if, "(if i (return i) (return null))") == 0);
	free(out_if);
	ast_destroy(branch);

	dstring_free(&name);
	printf("PASSED\n");
}

static void test_null_and_edge_cases(void) {
	printf("Running test_null_and_edge_cases... ");

	ast_destroy(NULL); /* must be a no-op */

	char *out = dump_to_string(NULL);
	assert(strcmp(out, "()") == 0);
	free(out);

	/* Invalid arguments must be rejected, not crash. */
	assert(ast_create_ident(NULL, 1, 1) == NULL);
	assert(ast_create_string(NULL, 1, 1) == NULL);
	assert(ast_create_call(NULL, 1, 1) == NULL);

	/* Ownership rule: the child is consumed even when the call fails. */
	assert(ast_create_var_decl(NULL, NULL, false, ast_create_int(1, 1, 1), 1, 1) == NULL);
	assert(ast_create_assignment(NULL, ast_create_int(1, 1, 1), 1, 1) == NULL);
	assert(ast_call_add_arg(NULL, ast_create_int(1, 1, 1)) == false);
	assert(ast_block_add_stmt(NULL, ast_create_int(1, 1, 1)) == false);

	/* Wrong node kind: also consumes the child. */
	ASTNode *not_a_block = ast_create_int(0, 1, 1);
	assert(ast_block_add_stmt(not_a_block, ast_create_int(1, 1, 1)) == false);
	assert(ast_call_add_arg(not_a_block, ast_create_int(1, 1, 1)) == false);
	ast_destroy(not_a_block);

	printf("PASSED\n");
}

/* ------------------------------------------------------------------ */
/* Fault injection: every allocation in the build is failed in turn.   */
/* ------------------------------------------------------------------ */

#if AST_TEST_HAVE_WRAP

/**
 * @brief Builds a representative tree, giving up cleanly on any failure.
 *
 * Because constructors own their arguments, the only cleanup needed here is
 * destroying the roots this function still holds itself.
 */
static ASTNode *build_sample_tree(void) {
	dstring_t fn_name, arg_name;
	ASTNode *block = NULL;

	if (!dstring_init(&fn_name)) {
		return NULL;
	}
	if (!dstring_init(&arg_name)) {
		dstring_free(&fn_name);
		return NULL;
	}
	dstring_append_str(&fn_name, "print");
	dstring_append_str(&arg_name, "val");

	block = ast_create_block(1, 1);
	if (!block) {
		goto done;
	}

	/* if (val > 0) print(val, val, val, val, val)  -- 5 args forces a realloc */
	ASTNode *cond = ast_create_binop(OP_GT,
	                                 ast_create_ident(&arg_name, 2, 9),
	                                 ast_create_int(0, 2, 15), 2, 13);

	ASTNode *call = ast_create_call(&fn_name, 2, 18);
	if (call) {
		for (int i = 0; i < 5; ++i) {
			if (!ast_call_add_arg(call, ast_create_ident(&arg_name, 2, 24))) {
				ast_destroy(call);
				call = NULL;
				break;
			}
		}
	}

	ASTNode *branch = ast_create_if(cond, call, NULL, 2, 5);
	if (!branch || !ast_block_add_stmt(block, branch)) {
		ast_destroy(block);
		block = NULL;
		goto done;
	}

	if (!ast_block_add_stmt(block, ast_create_return(ast_create_null(3, 12), 3, 5))) {
		ast_destroy(block);
		block = NULL;
	}

done:
	dstring_free(&fn_name);
	dstring_free(&arg_name);
	return block;
}

static void test_oom_paths(void) {
	printf("Running test_oom_paths...\n");

	/* Baseline: how many allocations does a full build take? */
	long before = g_live_blocks;
	ASTNode *full = build_sample_tree();
	assert(full != NULL);
	ast_destroy(full);
	assert(g_live_blocks == before);

	long checked = 0;
	for (long i = 0; i < 512; ++i) {
		long baseline = g_live_blocks;

		g_injection_fired = false;
		g_fail_countdown = i;
		ASTNode *tree = build_sample_tree();
		g_fail_countdown = -1;

		ast_destroy(tree);

		if (g_live_blocks != baseline) {
			printf("  FAILED: leak of %ld block(s) when allocation #%ld fails\n",
			       g_live_blocks - baseline, i);
			fflush(stdout);
			abort();
		}
		checked = i + 1;

		/* Injection never fired -> we are past the last allocation. */
		if (!g_injection_fired) {
			break;
		}
	}

	printf("  %ld allocation failure point(s) checked, no leaks\n", checked);
	printf("Running test_oom_paths... PASSED\n");
}

#else /* !AST_TEST_HAVE_WRAP */

static void test_oom_paths(void) {
	printf("Running test_oom_paths... SKIPPED (build without -Wl,--wrap)\n");
}

#endif /* AST_TEST_HAVE_WRAP */

/* ------------------------------------------------------------------ */

int main(void) {
	printf("=== Starting AST Test Suite ===\n");

	/* Warm up stdio buffers before any allocation accounting starts. */
	fflush(stdout);

	test_literals_and_binop();
	test_unary_and_string_literal();
	test_var_decl_and_assignment();
	test_complex_block_and_control_flow();
	test_nodelist_growth();
	test_while_and_if_else();
	test_null_and_edge_cases();
	test_oom_paths();

	printf("=== All tests passed successfully ===\n");
	return 0;
}
