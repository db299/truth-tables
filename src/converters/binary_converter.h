#pragma once
/**
 * Function to convert an int to a binary string, padding with 0s 
 * to ensure the string respects the number of variables in the truth table.
 * Caller is responsible for freeing memory allocated for the converted string returned.
 * @param number The number that needs to be converted to a binary string
 * @param length The total length of the number. The binary conversion will 
 * be padded with zeros to reach this length
 * @return The number after the conversion as a string
*/
char *int_to_binary_string(int number, int length);

/**
 * Function that takes in a rpn expression and replaces the alphabetic 
 * characters with digits from a binary string. For example, in 
 * ab|c| run with 3 (011 in binary) would become 01|1|.
 * Caller is responsible for freeing the memory allocated for the converted string returned.
 * @param original_expression The rpn expression before the conversion
 * @param binary_number The number whose binary representation will be 
 * used to replace alphabetic characters
 * @return The expression now representing a constant which can be evaluated
*/
char *replace_with_binary(const char *original_expression, int binary_number);

/**
 * Uses the infix map to reshuffle rpn into the infix expression it was converted from
 * by checking where each operator is mapped from in the infix expression, and matching with the result from
 * the evaluated rpn expression. The algorithm is almost identical to shunting yard, except this time we are keeping 
 * track of where each element from the input is going in the output.
 * Caller is responsible for freeing the memory allocated for the expression returned.
 * @param map The map used to convert the evaluated expression back to infix
 * @param rpn The evaluated rpn expression
 * @param map_size The length of map
 * @return The infix expression resulting from remapping rpn using map
 */
char *convert_evaled_rpn_to_infix(const int *map, const char *rpn, int map_size);