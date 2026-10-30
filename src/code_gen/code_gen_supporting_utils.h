/**
 * @project IFJ26 Compiler
 * @file code_gen_supporting_utils.h
 * @brief Code gen supporting utils module interface for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_CODE_GEN_SUPPORTING_UTILS_H
#define IFJ26_CODE_GEN_SUPPORTING_UTILS_H

#include <stdio.h>

/**
 * @brief Emits definition of a variable in the specified frame.
 * @param frame Frame name (e.g., "LF", "GF", "TF").
 * @param var_name Variable name to define.
 */
void gen_defvar(const char *frame, const char *var_name);

/**
 * @brief Emits a move instruction to assign a value from one variable to another.
 * @param dst_frame Destination frame name (e.g., "LF", "GF", "TF").
 * @param dst_name Destination variable name.
 * @param src_frame Source frame name (e.g., "LF", "GF", "TF").
 * @param src_name Source variable name.
 */
void gen_move_var(
	const char *dst_frame, const char *dst_name, const char *src_frame, const char *src_name);

/**
 * @brief Emits a move instruction to assign an integer value to a variable.
 * @param dst_frame Destination frame name (e.g., "LF", "GF", "TF").
 * @param dst_name Destination variable name.
 * @param value Integer value to assign.
 */
void gen_move_int(const char *dst_frame, const char *dst_name, int value);

/**
 * @brief Emits a move instruction to assign a string value to a variable.
 * @param dst_frame Destination frame name (e.g., "LF", "GF", "TF").
 * @param dst_name Destination variable name.
 * @param value String value to assign.
 */
void gen_move_string(const char *dst_frame, const char *dst_name, const char *value);

/**
 * @brief Emits an arithmetic operation instruction.
 * @param dst_frame Destination frame name (e.g., "LF", "GF", "TF").
 * @param dst_name Destination variable name.
 * @param op Arithmetic operation (e.g., "ADD", "SUB", "MUL", "DIV").
 * @param src_frame1 Source frame name for the first operand.
 * @param src_name1 Source variable name for the first operand.
 * @param src_frame2 Source frame name for the second operand.
 * @param src_name2 Source variable name for the second operand.
 */
void gen_arithmetic(const char *dst_frame, const char *dst_name, const char *op,
	const char *src_frame1, const char *src_name1, const char *src_frame2, const char *src_name2);

/**
 * @brief Emits a label definition instruction.
 * @param label_name Name of the label.
 * @param label_index Index of the label (used for uniqueness).
 */
void gen_label(const char *label_name, int label_index);

/**
 * @brief Emits a jump instruction to a label.
 * @param label_name Name of the label to jump to.
 * @param label_index Index of the label (used for uniqueness).
 */
void gen_jump(const char *label_name, int label_index);

/**
 * @brief Emits a conditional jump instruction to a label if two variables are equal.
 * @param label_name Name of the label to jump to.
 * @param label_index Index of the label (used for uniqueness).
 * @param src_frame1 Source frame name for the first variable.
 * @param src_name1 Source variable name for the first variable.
 * @param src_frame2 Source frame name for the second variable.
 * @param src_name2 Source variable name for the second variable.
 */
void gen_jump_if_eq(const char *label_name, int label_index, const char *src_frame1,
	const char *src_name1, const char *src_frame2, const char *src_name2);

#endif /* IFJ26_CODE_GEN_SUPPORTING_UTILS_H */
