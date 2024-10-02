#pragma once
#include <semaphore.h>

/**
 * Function to generate a header for the table.
 * Generates the header irrespective of expression type
 * @param expression The expression corresponding to the header
 * @return The header
 */
char *generate_header(const char *expression);

/**
 * Function to generate a separator for the table.
 * Generates the separator irrespective of expression type.
 * Expression is only needed to figure out the required length
 * @param expression The expression corresponding to the separator
 * @return The separator
 */
char *generate_separator(const char *expression);

/**
 * Function to generate a postfix (rpn) row.
 * This is done by evaluating a constant logical expression corresponding
 * to the binary row number at the given expression.
 * @param row_number At row number 3 (011) with variables a b c, the expression would evaluate at a=0 b=1 c=1
 * @param number_of_variables The number of variables in the expression.
 * @param expression The expression itself
 * @param expr_length The length of the expression
 * @return The full evaluated row including final result, intermediate results, and variable values
 */
char *generate_postfix_row(int row_number, int number_of_variables, const char *expression, int expr_length);

/**
 * Function to generate the full table body (including header and separator), followed by writing it to a file
 * @param expression postfix expression for which the table body is generated
 * @param file the file where the table body is written
 * @return void, the function just writes the table to a text file
 */
void generate_postfix_table_body(const char *expression, FILE *file);

/**
 * Function to generate the full table body (including header and separator), followed by writing it to a file
 * @param expression the infix expression for which the table body is generated
 * @param file the file where the table body is written
 * @return void, the function just writes the table to a text file
 */
void generate_infix_table_body(const char *expression, FILE *file);

/**
 * Function passed to threads to generate a segment of the postfix truth table.
 * @param arg A struct containing all information required to generate a segment of the table
 * @return void, no return value, the function just writes the segment to a text file given in
 * arg
 */
void *postfix_rows_generator(void *arg);

/**
 * Function passed to threads to generate a segment of the infix truth table.
 * @param arg A struct containing all information required to generate a segment of the table
 * @return void, no return value, the function just writes the segment to a text file given in
 * arg
 */
void *infix_rows_generator(void *arg);

/**
 * Function to generate an infix row.
 * This is done by evaluating a constant logical expression corresponding
 * to the binary row number at the given expression.
 * @param row_number At row number 3 (011) with variables a b c, the expression would evaluate at at a=0 b=1 c=1
 * @param number_of_variables The number of variables in the expression.
 * @param expression The expression (in rpn) for which the segment is generated.
 * @param map The map used to reshuffle the rpn expression to infix.
 * @param expr_length The length of the infix expression
 * @param rpn_length The length of the rpn expression
 * @return The full evaluated row including final result, intermediate results, and variable values
 */
char *generate_infix_row(int row_number, int number_of_variables, const char *expression, int *map, int expr_length, int rpn_length);

/**
 * Function to generate a segment of the table for a postfix expression between start_row and end_row
 * so the range [start_row, end_row), meaning not inclusive of end_row
 * @param expression the expression for which the segment is generated
 * @param start_row start row for generation
 * @param end_row end row for generation
 * @return The generated segment
 */
char *generate_postfix_truth_table_segment(const char *expression, int start_row, int end_row);

/**
 * Function to generate a segment of the table for an infix expression between start_row and end_row
 * so the range [start_row, end_row), meaning not inclusive of end_row
 * @param expression the expression for which the segment is generated
 * @param start_row start row for generation
 * @param end_row end row for generation
 * @return The generated segment
 */
char *generate_infix_truth_table_segment(const char *expression, int start_row, int end_row);

/**
 * Function to generate only the true rows in a truth table segment for an infix expression. Allocates the memory
 * the entire segment would have taken as an upper bound.
 * @param expression the expression (in rpn) for which the segment is generated
 * @param infix_map the map to reshuffle the rpn expression into infix
 * @param expression_length the length of the infix expression
 * @param rpn_length the length of the rpn expression
 * @param number_of_variables the number of variables in the expression
 * @param start_row the start row for the segment
 * @param end_rows the end row for the segment
 */
char *generate_true_infix_truth_table_segment(const char *rpn_expression, int *infix_map, int expression_length, int rpn_length, int number_of_variables, int start_row, int end_row);

/**
 * Function to generate only the true rows in a truth table segment for a postfix expression. Allocates the memory
 * the entire segment would have taken as an upper bound.
 * @param expression the expression for which the segment is generated
 * @param expr_length the length of the expression
 * @param number_of_variables the number of variables in the expression
 * @param start_row the start row for the segment
 * @param end_row the end row for the segment
 */
char *generate_true_postfix_truth_table_segment(const char *expression, int expr_length, int number_of_variables, int start_row, int end_row);

/**
 * Struct holding data for postfix row generator threads
 */
typedef struct
{
    FILE *file;
    sem_t *semaphore;
    sem_t *creation_semaphore;
    int num_threads;
    int thread_id;
    int number_of_variables;
    int expression_length;
    const char *expression;
    int start_row;
    int end_row;
} postfix_thread_data;

/**
 * Struct holding data for infix row generator threads
 */
typedef struct
{
    FILE *file;
    sem_t *semaphore;
    sem_t *creation_semaphore;
    int num_threads;
    int thread_id;
    int number_of_variables;
    int *map;
    const char *expression;
    const char *rpn_expression;
    int rpn_length;
    int expression_length;
    int start_row;
    int end_row;
} infix_thread_data;