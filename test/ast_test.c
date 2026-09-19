/**
 * @file ast_test.c
 * @brief Unit tests for AST construction, S-expression dump, and cleanup.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/parser/ast.h"
#include "../src/_utils/dstring.h"

// Pomocná funkce pro zachycení S-výrazu do stringu
static char *dump_to_string(const ASTNode *node) {
    char *bp = NULL;
    size_t size = 0;
    FILE *stream = open_memstream(&bp, &size);
    assert(stream != NULL);

    ast_dump_sexpr(node, stream);
    fclose(stream);

    return bp;
}

static void test_literals_and_binop(void) {
    printf("Running test_literals_and_binop... ");

    // Výraz: 42 + 3.14
    ASTNode *lit_int = ast_create_int(42, 1, 1);
    ASTNode *lit_float = ast_create_float(3.14, 1, 6);
    ASTNode *binop = ast_create_binop(OP_ADD, lit_int, lit_float, 1, 4);

    char *out = dump_to_string(binop);
    assert(strcmp(out, "(+ 42 3.14)") == 0);
    free(out);

    ast_destroy(binop);
    printf("PASSED\n");
}

static void test_var_decl_and_assignment(void) {
    printf("Running test_var_decl_and_assignment... ");

    dstring_t var_name, type_name;
    dstring_init(&var_name);
    dstring_init(&type_name);
    dstring_append_str(&var_name, "x");
    dstring_append_str(&type_name, "i32");

    // var x:i32 = 10
    ASTNode *init_val = ast_create_int(10, 1, 15);
    ASTNode *decl = ast_create_var_decl(&var_name, &type_name, false, init_val, 1, 1);

    char *out_decl = dump_to_string(decl);
    assert(strcmp(out_decl, "(var x:i32 10)") == 0);
    free(out_decl);
    ast_destroy(decl);

    // Přiřazení: x = x + 1
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
    dstring_init(&fn_name);
    dstring_init(&arg_name);
    dstring_append_str(&fn_name, "print");
    dstring_append_str(&arg_name, "val");

    // Block:
    // {
    //    if (val > 0) print(val);
    //    return null;
    // }
    ASTNode *block = ast_create_block(1, 1);

    // Podmínka: val > 0
    ASTNode *val_ident = ast_create_ident(&arg_name, 2, 9);
    ASTNode *zero_lit = ast_create_int(0, 2, 15);
    ASTNode *cond = ast_create_binop(OP_GT, val_ident, zero_lit, 2, 13);

    // Volání: print(val)
    ASTNode *call = ast_create_call(&fn_name, 2, 18);
    ast_call_add_arg(call, ast_create_ident(&arg_name, 2, 24));

    // if (cond) call
    ASTNode *if_node = ast_create_if(cond, call, NULL, 2, 5);
    ast_block_add_stmt(block, if_node);

    // return null
    ASTNode *ret_node = ast_create_return(ast_create_null(3, 12), 3, 5);
    ast_block_add_stmt(block, ret_node);

    char *out = dump_to_string(block);
    const char *expected = "(block (if (> val 0) (call print val)) (return null))";
    assert(strcmp(out, expected) == 0);

    free(out);
    ast_destroy(block);

    dstring_free(&fn_name);
    dstring_free(&arg_name);
    printf("PASSED\n");
}

int main(void) {
    printf("=== Starting AST Test Suite ===\n");
    test_literals_and_binop();
    test_var_decl_and_assignment();
    test_complex_block_and_control_flow();
    printf("=== All tests passed successfully ===\n");
    return 0;
}
