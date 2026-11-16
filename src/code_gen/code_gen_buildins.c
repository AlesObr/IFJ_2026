/**
 * @project IFJ26 Compiler
 * @file code_gen_buildins.c
 * @brief Code gen buildins module implementation for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11
 */

#include <stdio.h>

void read_line(void) {
	printf("LABEL $built_in_readLine\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("READ LF@%%retval string\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

void read_int(void) {
	printf("LABEL $built_in_readInt\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("READ LF@%%retval int\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

void read_double(void) {
	printf("LABEL $built_in_readDouble\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("READ LF@%%retval float\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

void print(void) {
	printf("LABEL $built_in_print\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("MOVE LF@%%retval nil@nil\n");
	printf("WRITE LF@%%arg1\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

void to_str(void) {
	printf("LABEL $built_in_toStr\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("DEFVAR LF@%%type\n");
	printf("TYPE LF@%%type LF@%%arg1\n");
	printf("JUMPIFEQ $built_in_toStr_int LF@%%type string@int\n");
	printf("JUMPIFEQ $built_in_toStr_float LF@%%type string@float\n");
	printf("JUMPIFEQ $built_in_toStr_bool LF@%%type string@bool\n");
	printf("MOVE LF@%%retval LF@%%arg1\n");
	printf("JUMP $built_in_toStr_end\n");
	printf("LABEL $built_in_toStr_int\n");
	printf("INT2STR LF@%%retval LF@%%arg1\n");
	printf("JUMP $built_in_toStr_end\n");
	printf("LABEL $built_in_toStr_float\n");
	printf("FLOAT2STR LF@%%retval LF@%%arg1\n");
	printf("JUMP $built_in_toStr_end\n");
	printf("LABEL $built_in_toStr_bool\n");
	printf("JUMPIFEQ $built_in_toStr_true LF@%%arg1 bool@true\n");
	printf("MOVE LF@%%retval string@false\n");
	printf("JUMP $built_in_toStr_end\n");
	printf("LABEL $built_in_toStr_true\n");
	printf("MOVE LF@%%retval string@true\n");
	printf("LABEL $built_in_toStr_end\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

void to_int(void) {
	printf("LABEL $built_in_toInt\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("FLOAT2INT LF@%%retval LF@%%arg1\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

void str_length(void) {
	printf("LABEL $built_in_strLength\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("STRLEN LF@%%retval LF@%%arg1\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

void sub_str(void) {
	printf("LABEL $built_in_subStr\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("DEFVAR LF@%%i\n");
	printf("DEFVAR LF@%%len\n");
	printf("DEFVAR LF@%%char\n");
	printf("DEFVAR LF@%%continue\n");
	printf("MOVE LF@%%retval string@\n");
	printf("MOVE LF@%%i LF@%%arg2\n");
	printf("STRLEN LF@%%len LF@%%arg1\n");
	printf("LABEL $built_in_subStr_loop\n");
	printf("LT LF@%%continue LF@%%i LF@%%arg3\n");
	printf("JUMPIFEQ $built_in_subStr_end LF@%%continue bool@false\n");
	printf("LT LF@%%continue LF@%%i LF@%%len\n");
	printf("JUMPIFEQ $built_in_subStr_end LF@%%continue bool@false\n");
	printf("GETCHAR LF@%%char LF@%%arg1 LF@%%i\n");
	printf("CONCAT LF@%%retval LF@%%retval LF@%%char\n");
	printf("ADD LF@%%i LF@%%i int@1\n");
	printf("JUMP $built_in_subStr_loop\n");
	printf("LABEL $built_in_subStr_end\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

void ord(void) {
	printf("LABEL $built_in_ord\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("STRI2INT LF@%%retval LF@%%arg1 LF@%%arg2\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

void chr(void) {
	printf("LABEL $built_in_chr\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("INT2CHAR LF@%%retval LF@%%arg1\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}
