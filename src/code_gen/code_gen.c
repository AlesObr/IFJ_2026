/**
 * @project IFJ26 Compiler
 * @file code_gen.c
 * @brief Code gen module implementation for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11
 */

#include "code_gen.h"
#include <stdio.h>

bool gen_binary_op_stack(OpType op, ValType result_type) {
	switch (op) {
		case OP_ADD:
			printf("ADDS\n");
			break;
		case OP_SUB:
			printf("SUBS\n");
			break;
		case OP_MUL:
			printf("MULS\n");
			break;
		case OP_DIV:
			if (result_type == INT)
				printf("IDIVS\n");
			else if (result_type == DOUBLE)
				printf("DIVS\n");
			else
				return false;
			break;
		case OP_LT:
			printf("LTS\n");
			break;
		case OP_GT:
			printf("GTS\n");
			break;
		case OP_EQ:
			printf("EQS\n");
			break;
		case OP_AND:
			printf("ANDS\n");
			break;
		case OP_OR:
			printf("ORS\n");
			break;
		case OP_NOT:
			printf("NOTS\n");
			break;
		default: // OP_CONCAT, OP_ASSIGN, OP_NEQ, OP_LTE, OP_GTE, OP_UNARY_MINUS, OP_UNARY_PLUS
			return false;
	}
	return true;
}

bool gen_expr(const ASTNode *exp) {
	if (!exp)
		return false;

	bool passed = true;
	switch (exp->type) {
		case NODE_BINARY_OP:
			if (!gen_expr(exp->op.left))
				return false;
			if (!gen_expr(exp->op.right))
				return false;
			if (!gen_binary_op_stack(exp->op.op, exp->expr_types))
				passed = false;
			break;
		case NODE_INT_LITERAL:
			printf("PUSHS int@%ld\n", exp->int_val);
			break;
		case NODE_FLOAT_LITERAL:
			printf("PUSHS float@%a\n", exp->float_val);
			break;
		case NODE_STRING_LITERAL:
			printf("PUSHS string@ ---NOT SUPPORTED YET---\n"); // Get the proper string
			passed = false;
			break;
		case NODE_NULL_LITERAL:
			printf("PUSHS nil@nil\n");
			break;
		case NODE_IDENTIFIER:
			printf("PUSHS LF@v%zu\n", exp->sym_id);
			break;
		default:
			passed = false;
			break;
	}

	if (exp->to_double)
		printf("INT2FLOATS\n");

	return passed;
}

bool gen_stmt(const ASTNode *stmt) {
	if (!stmt)
		return false;

	switch (stmt->type) {
		case NODE_VAR_DECL:
			if (!gen_expr(stmt->var_decl.init_expr))
				return false;
			printf("POPS LF@v%zu\n", stmt->sym_id);
			return true;
		case NODE_BINARY_OP:
			if (stmt->op.op == OP_ASSIGN) {
				if (!gen_expr(stmt->op.right))
					return false;
				printf("POPS LF@v%zu\n", stmt->op.left->sym_id);
				return true;
			}
			// expressions like 1 + 5 without assignment
			if (!gen_expr(stmt))
				return false;
			printf("POPS GF@throwaway\n");
			return true;
		case NODE_INT_LITERAL:
		case NODE_FLOAT_LITERAL:
		case NODE_STRING_LITERAL:
		case NODE_NULL_LITERAL:
		case NODE_CALL:
		case NODE_UNARY_OP:
		case NODE_IDENTIFIER:
			if (!gen_expr(stmt))
				return false;
			printf("POPS GF@throwaway\n");
			break;
		case NODE_BLOCK:
			for (size_t i = 0; i < stmt->block.stmts.count; i++) {
				ASTNode *block_stmt = stmt->block.stmts.items[i];
				if (!gen_stmt(block_stmt))
					return false;
			}
			break;
		default:
			return false; // if, while, return, for
	}
	return true;
}