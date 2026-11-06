/**
 * @project IFJ26 Compiler
 * @file semantic.c
 * @brief Semantic module implementation for IFJ26 compiler.
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11
 */

#include "semantic.h"

static void set_type(ASTNode *n, ValType t) {
	(void)n;
	(void)t; /* fields not in ast.h */
}
static void mark_convert(ASTNode *n) {
	(void)n; /* fields not in ast.h */
}

Error analyze_expression(SemanticContext *ctx, ASTNode *expr, ValType *out) {
	if (!ctx || !expr || !out) {
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters to analyze_expression");
	}

	switch (expr->type) {
		case NODE_INT_LITERAL:
			*out = INT;
			break;
		case NODE_FLOAT_LITERAL:
			*out = DOUBLE;
			break;
		case NODE_STRING_LITERAL:
			*out = STRING;
			break;
		case NODE_NULL_LITERAL: // FOR FUTURE ME: confirm this please
			*out = UNIT;		// Assuming NULL is treated as UNIT type
			break;
		case NODE_IDENTIFIER: {
			Symbol *sym = scope_lookup(&ctx->scope_stack, dstring_get(&expr->str_val));
			if (!sym) {
				return ERROR_EMIT(ERR_SEM_UNDEF, expr->line, expr->col, "Undefined variable");
			}
			*out = sym->type;
			break;
		}
		case NODE_BINARY_OP: {
			ValType left_type, right_type;
			bool conv_left, conv_right;

			Error err = analyze_expression(ctx, expr->op.left, &left_type);
			if (ERROR_IS_ERR(err))
				return err;

			err = analyze_expression(ctx, expr->op.right, &right_type);
			if (ERROR_IS_ERR(err))
				return err;

			*out = binop_result(expr->op.op, left_type, right_type, &conv_left, &conv_right);
			if (*out == UNDEF) {
				return ERROR_EMIT(
					ERR_SEM_TYPE, expr->line, expr->col, "Type error in binary operation");
			}

			// Handle conversions
			if (conv_left)
				mark_convert(expr->op.left);
			if (conv_right)
				mark_convert(expr->op.right);
			break;
		}
		case NODE_UNARY_OP: {
			ValType operand_type;
			Error err = analyze_expression(ctx, expr->op.left, &operand_type);
			if (ERROR_IS_ERR(err))
				return err;

			*out = unop_result(expr->op.op, operand_type);
			if (*out == UNDEF) {
				return ERROR_EMIT(
					ERR_SEM_TYPE, expr->line, expr->col, "Type error in unary operation");
			}

			// Handle conversions
			break;
		}
		default: // Temporary default case for unsupported expression types
			return ERROR_EMIT(
				ERR_INTERNAL, expr->line, expr->col, "Unsupported expression type for analysis");
	}

	set_type(expr, *out); // Record the type of the expression node
	return ERROR_OK();
}
