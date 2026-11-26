/**
 * @project IFJ26 Compiler
 * @file type_rules.c
 * @brief Type checking rules for semantic analysis.
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11
 */

#include "type_rules.h"

static bool is_numeric(ValType t) {
	return t == INT || t == DOUBLE;
}

bool type_compatible(ValType expected, ValType actual, bool *convert) {
	*convert = false;
	switch (expected) {
		case INT:
			return actual == INT;
		case DOUBLE:
			*convert = (actual == INT);
			return actual == DOUBLE || actual == INT;
		case STRING:
			return actual == STRING;
		case BOOLEAN:
			return actual == BOOLEAN;
		case UNIT:
			return actual == UNIT;
		case ANY:
			return true;
		default:
			break;
	}
	return false;
}

ValType binop_result(OpType op, ValType l, ValType r, bool *conv_l, bool *conv_r) {
	*conv_l = false;
	*conv_r = false;

	// Check if both operands are numeric (INT or DOUBLE)
	bool l_is_num = is_numeric(l);
	bool r_is_num = is_numeric(r);

	bool l_int_r_double = (l == INT && r == DOUBLE); // Left is Int, Right is Double
	bool l_double_r_int = (r == INT && l == DOUBLE); // Left is Double, Right is Int

	switch (op) {
		case OP_ADD:
		case OP_SUB:
		case OP_MUL:
		case OP_DIV:
			if (l_is_num && r_is_num) {
				*conv_l = l_int_r_double;
				*conv_r = l_double_r_int;
				return (l == DOUBLE || r == DOUBLE) ? DOUBLE : INT;
			}
			if (op == OP_ADD && l == STRING && r == STRING) {
				return STRING;
			}
			break;
		case OP_LT:
		case OP_GT:
		case OP_LTE:
		case OP_GTE:
			if (l_is_num && r_is_num) {
				*conv_l = l_int_r_double;
				*conv_r = l_double_r_int;
				return BOOLEAN;
			}
			if (l == STRING && r == STRING) {
				return BOOLEAN;
			}
			break;
		case OP_EQ:
		case OP_NEQ:
			if (l == r && l != UNDEF) {
				return BOOLEAN;
			}
			if ((l_int_r_double) || (l_double_r_int)) {
				*conv_l = l_int_r_double;
				*conv_r = l_double_r_int;
				return BOOLEAN;
			}
			break;
		case OP_OR:
		case OP_AND:
			if (l == BOOLEAN && r == BOOLEAN)
				return BOOLEAN;
			break;
		default:
			break;
	}
	return UNDEF;
}

ValType unop_result(OpType op, ValType operand) {
	switch (op) {
		case OP_UNARY_PLUS:
		case OP_UNARY_MINUS:
			if (is_numeric(operand)) {
				return operand;
			}
			break;
		case OP_NOT:
			if (operand == BOOLEAN) {
				return BOOLEAN;
			}
			break;
		default:
			break;
	}
	return UNDEF;
}

Error type_from_nodes(const ASTNodeList *types, size_t line, size_t col, ValType *out) {
	if (types->count == 0) {
		*out = UNDEF;
		return ERROR_OK();
	}
	if (types->count > 1)
		return ERROR_EMIT(ERR_INTERNAL, line, col, "Union types are not suppoerted yet"); // ALGEBDT

	ASTNode *t = types->items[0];
	if (t->type == NODE_TYPE_ORIG) {
		*out = t->data_type.orig_type;
		return ERROR_OK();
	}

	return ERROR_EMIT(ERR_SEM_UNDEF, line, col, "Undefined type"); // ALGEBDT
}