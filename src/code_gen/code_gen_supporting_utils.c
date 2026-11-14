/**
 * @project IFJ26 Compiler
 * @file code_gen_supporting_utils.c
 * @brief Code gen supporting utils module implementation for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#include "code_gen_supporting_utils.h"
#include <stdio.h>

void gen_defvar(const char *frame, const char *var_name) {
	printf("DEFVAR %s@%s\n", frame, var_name);
}

void gen_move_var(
	const char *dst_frame, const char *dst_name, const char *src_frame, const char *src_name) {
	printf("MOVE %s@%s %s@%s\n", dst_frame, dst_name, src_frame, src_name);
}

void gen_move_int(const char *dst_frame, const char *dst_name, int value) {
	printf("MOVE %s@%s INT@%d\n", dst_frame, dst_name, value);
}

void gen_move_string(const char *dst_frame, const char *dst_name, const char *value) {
	printf("MOVE %s@%s STRING@%s\n", dst_frame, dst_name, value);
}

void gen_label(const char *prefix, int label_index) {
	printf("LABEL $%s_%d\n", prefix, label_index);
}

void gen_jump(const char *prefix, int label_index) {
	printf("JUMP $%s_%d\n", prefix, label_index);
}

void gen_jumpifeq(const char *prefix, int id, const char *cmp_frame, const char *cmp_name,
	const char *type, const char *value) {
	printf("JUMPIFEQ $%s_%d %s@%s %s@%s\n", prefix, id, cmp_frame, cmp_name, type, value);
}

void gen_arithmetic(const char *instr, const char *dst_frame, const char *dst_name,
	const char *op1_frame, const char *op1_name, const char *op2_frame, const char *op2_name) {
	printf("%s %s@%s %s@%s %s@%s\n", instr, dst_frame, dst_name, op1_frame, op1_name, op2_frame,
		op2_name);
}
