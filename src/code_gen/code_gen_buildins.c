/**
 * @project IFJ26 Compiler
 * @file code_gen_buildins.c
 * @brief Code gen buildins module implementation for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11
 *
 * Every built-in is emitted as an IFJcode26 function behind the label $built_in_<name>.
 * Calling convention: the caller creates a temporary frame with the arguments
 * TF@%arg1, TF@%arg2, ... and calls the label; the result is left in LF@%retval, which
 * becomes TF@%retval for the caller after POPFRAME. The value () of type Unit is
 * represented by nil@nil.
 *
 * Behaviour follows section 4 of the IFJ26 specification and the reference ifj26.scala.
 */

#include <stdio.h>

void read_line(void) {
	printf("LABEL $built_in_readLine\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("DEFVAR LF@%%type\n");
	printf("READ LF@%%retval string\n");
	// READ returns nil at EOF, readLine returns an empty string there
	printf("TYPE LF@%%type LF@%%retval\n");
	printf("JUMPIFNEQ $built_in_readLine_end LF@%%type string@nil\n");
	printf("MOVE LF@%%retval string@\n");
	printf("LABEL $built_in_readLine_end\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

void read_int(void) {
	printf("LABEL $built_in_readInt\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("DEFVAR LF@%%type\n");
	printf("READ LF@%%retval int\n");
	// READ returns nil on invalid input or EOF, readInt returns 0 there
	printf("TYPE LF@%%type LF@%%retval\n");
	printf("JUMPIFNEQ $built_in_readInt_end LF@%%type string@nil\n");
	printf("MOVE LF@%%retval int@0\n");
	printf("LABEL $built_in_readInt_end\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

void read_double(void) {
	printf("LABEL $built_in_readDouble\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("DEFVAR LF@%%type\n");
	printf("READ LF@%%retval float\n");
	// READ returns nil on invalid input or EOF, readDouble returns 0.0 there
	printf("TYPE LF@%%type LF@%%retval\n");
	printf("JUMPIFNEQ $built_in_readDouble_end LF@%%type string@nil\n");
	printf("MOVE LF@%%retval float@0x0p+0\n");
	printf("LABEL $built_in_readDouble_end\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

void read_boolean(void) {
	printf("LABEL $built_in_readBoolean\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("READ LF@%%retval bool\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

void print(void) {
	printf("LABEL $built_in_print\n");
	printf("PUSHFRAME\n");
	// print returns () like any Unit function (its result may be used as a value)
	printf("DEFVAR LF@%%retval\n");
	printf("MOVE LF@%%retval nil@nil\n");
	printf("DEFVAR LF@%%type\n");
	// WRITE prints nil as "nil", but the Unit value () is printed as "()"
	printf("TYPE LF@%%type LF@%%arg1\n");
	printf("JUMPIFEQ $built_in_print_unit LF@%%type string@nil\n");
	printf("WRITE LF@%%arg1\n");
	printf("JUMP $built_in_print_end\n");
	printf("LABEL $built_in_print_unit\n");
	printf("WRITE string@()\n");
	printf("LABEL $built_in_print_end\n");
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
	printf("JUMPIFEQ $built_in_toStr_unit LF@%%type string@nil\n");
	printf("MOVE LF@%%retval LF@%%arg1\n");
	printf("JUMP $built_in_toStr_end\n");
	printf("LABEL $built_in_toStr_int\n");
	printf("INT2STR LF@%%retval LF@%%arg1\n");
	printf("JUMP $built_in_toStr_end\n");
	printf("LABEL $built_in_toStr_float\n");
	// FLOAT2STR uses the "%.2f" format required by toStr (WRITE would use %a)
	printf("FLOAT2STR LF@%%retval LF@%%arg1\n");
	printf("JUMP $built_in_toStr_end\n");
	printf("LABEL $built_in_toStr_unit\n");
	printf("MOVE LF@%%retval string@()\n");
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
	printf("DEFVAR LF@%%type\n");
	// An Int argument is accepted through the implicit Int -> Double widening; if the caller
	// did not convert it, FLOAT2INT would fail on it, so it is returned unchanged
	printf("TYPE LF@%%type LF@%%arg1\n");
	printf("JUMPIFEQ $built_in_toInt_int LF@%%type string@int\n");
	printf("FLOAT2INT LF@%%retval LF@%%arg1\n");
	printf("JUMP $built_in_toInt_end\n");
	printf("LABEL $built_in_toInt_int\n");
	printf("MOVE LF@%%retval LF@%%arg1\n");
	printf("LABEL $built_in_toInt_end\n");
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
	printf("DEFVAR LF@%%cond\n");
	printf("STRLEN LF@%%len LF@%%arg1\n");
	// Runtime error 40 if start < 0, end < 0, start > end, start >= length or end > length
	printf("LT LF@%%cond LF@%%arg2 int@0\n");
	printf("JUMPIFEQ $built_in_subStr_error LF@%%cond bool@true\n");
	printf("LT LF@%%cond LF@%%arg3 int@0\n");
	printf("JUMPIFEQ $built_in_subStr_error LF@%%cond bool@true\n");
	printf("GT LF@%%cond LF@%%arg2 LF@%%arg3\n");
	printf("JUMPIFEQ $built_in_subStr_error LF@%%cond bool@true\n");
	printf("LT LF@%%cond LF@%%arg2 LF@%%len\n");
	printf("JUMPIFEQ $built_in_subStr_error LF@%%cond bool@false\n");
	printf("GT LF@%%cond LF@%%arg3 LF@%%len\n");
	printf("JUMPIFEQ $built_in_subStr_error LF@%%cond bool@true\n");
	printf("MOVE LF@%%retval string@\n");
	printf("MOVE LF@%%i LF@%%arg2\n");
	printf("LABEL $built_in_subStr_loop\n");
	printf("JUMPIFEQ $built_in_subStr_end LF@%%i LF@%%arg3\n");
	printf("GETCHAR LF@%%char LF@%%arg1 LF@%%i\n");
	printf("CONCAT LF@%%retval LF@%%retval LF@%%char\n");
	printf("ADD LF@%%i LF@%%i int@1\n");
	printf("JUMP $built_in_subStr_loop\n");
	printf("LABEL $built_in_subStr_error\n");
	printf("EXIT int@40\n");
	printf("LABEL $built_in_subStr_end\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

void ord(void) {
	printf("LABEL $built_in_ord\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("DEFVAR LF@%%len\n");
	printf("DEFVAR LF@%%cond\n");
	// Empty string or index out of range gives -1 (STRI2INT would end with error 58)
	printf("MOVE LF@%%retval int@-1\n");
	printf("STRLEN LF@%%len LF@%%arg1\n");
	printf("LT LF@%%cond LF@%%arg2 int@0\n");
	printf("JUMPIFEQ $built_in_ord_end LF@%%cond bool@true\n");
	printf("LT LF@%%cond LF@%%arg2 LF@%%len\n");
	printf("JUMPIFEQ $built_in_ord_end LF@%%cond bool@false\n");
	printf("STRI2INT LF@%%retval LF@%%arg1 LF@%%arg2\n");
	printf("LABEL $built_in_ord_end\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

void chr(void) {
	printf("LABEL $built_in_chr\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	// A code outside 0..127 ends with error 58 of INT2CHAR, as the specification requires
	printf("INT2CHAR LF@%%retval LF@%%arg1\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}
