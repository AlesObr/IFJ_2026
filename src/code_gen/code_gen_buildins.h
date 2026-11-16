/**
 * @project IFJ26 Compiler
 * @file code_gen_buildins.h
 * @brief Built-in routine emitters for the IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_CODE_GEN_BUIDLINS_H
#define IFJ26_CODE_GEN_BUIDLINS_H

/**
 * @brief Emits the code for the built-in function readLine.
 */
void read_line(void);

/**
 * @brief Emits the code for the built-in function readInt.
 */
void read_int(void);

/**
 * @brief Emits the code for the built-in function readDouble.
 */
void read_double(void);

/**
 * @brief Emits the code for the built-in function print.
 */
void print(void);

/**
 * @brief Emits the code for the built-in function toStr.
 */
void to_str(void);

/**
 * @brief Emits the code for the built-in function toInt.
 */
void to_int(void);

/**
 * @brief Emits the code for the built-in function strLength.
 */
void str_length(void);

/**
 * @brief Emits the code for the built-in function subStr.
 */
void sub_str(void);

/**
 * @brief Emits the code for the built-in function ord.
 */
void ord(void);

/**
 * @brief Emits the code for the built-in function chr.
 */
void chr(void);

#endif /* IFJ26_CODE_GEN_BUIDLINS_H */
