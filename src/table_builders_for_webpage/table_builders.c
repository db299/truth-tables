#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>

#include "../rpn_evaluator/evaluation.h"
#include "../converters/binary_converter.h"
#include "../converters/shunting_yard.h"
#include "../utils/find_nr_of_vars.h"

#include "table_builders.h"

char *generate_postfix_row(int row_number, int number_of_variables, const char *expression) {
    int expression_length = strlen(expression);

    int row_length = number_of_variables * 2 + expression_length + 10;
    char *row = (char *)malloc((row_length) * sizeof(char));
    if (row == NULL) {
        fprintf(stderr, "Memory allocation for postfix row failed in %s at line %d\n", __FILE__, __LINE__);
        return (char *)NULL;
    }

    char *binary_number_string = int_to_binary_string(row_number, number_of_variables);
    if (binary_number_string == NULL) {
        fprintf(stderr, "Binary conversion failed in %s at line %d\n", __FILE__, __LINE__);
        free(row);
        return (char *)NULL;
    }

    // Copy binary values to the row
    for (int j = 0; j < number_of_variables; j++) {
        row[2 * j] = binary_number_string[j];
        row[2 * j + 1] = ' '; // Add a space after each binary digit
    }
    row[number_of_variables * 2] = ':';  // Add the colon
    row[number_of_variables * 2 + 1] = ' '; // Add space after colon

    char *modified_expression = replace_with_binary(expression, row_number);
    if (modified_expression == NULL) {
        fprintf(stderr, "Failed to replace variables with binary in %s at line %d\n", __FILE__, __LINE__);
        free(row);
        free(binary_number_string);
        return (char *)NULL;
    }

    char *result = evaluate_expr(modified_expression);
    if (result == NULL) {
        fprintf(stderr, "Evaluation failed in %s at line %d\n", __FILE__, __LINE__);
        free(modified_expression);
        free(binary_number_string);
        return (char *)NULL;
    }

    int position = number_of_variables * 2 + 2;
    // Handle single variable case
    if (expression_length == 1) { // number_of_variables*2 for the first column, +2 for ": "
        // Copy the string "  :   "
        row[position++] = ' ';
        row[position++] = ' ';
        row[position++] = ':';
        row[position++] = ' ';
        row[position++] = ' ';
        row[position++] = ' ';
        row[position++] = modified_expression[0]; // Copy the modified expression
        row[position++] = '\n'; // New line at the end
    } else {
        int result_length = strlen(result);
        memcpy(row + position, result, result_length); // Copy result
        position += result_length;
        row[position++] = ' '; // Space before the final result
        row[position++] = ':'; // Add the colon
        row[position++] = ' '; // Space after colon
        row[position++] = ' '; // Space after colon
        row[position++] = ' '; // Space after colon
        row[position++] = result[result_length-1]; // Last character of result
        row[position++] = '\n'; // New line at the end
        row[position++] = '\0'; // Null-terminate the string
    }

    free(result);
    free(modified_expression);
    free(binary_number_string);
    return row;
}

char *generate_postfix_truth_table_segment(const char *expression, int start_row, int end_row)
{
    // Making sure not to overshoot the table
    if (end_row >= (1 << count_unique_variables(expression)))
    {
        end_row = (1 << count_unique_variables(expression));
    }

    if (start_row < 0)
    {
        fprintf(stderr, "Failed to generate segment in file %s at line %d: invalid start row %d\n", __FILE__, __LINE__, start_row);
        return (char *)NULL;
    }
    if (end_row < 0)
    {
        fprintf(stderr, "Failed to generate segment in file %s at line %d: invalid end row %d\n", __FILE__, __LINE__, end_row);
        return (char *)NULL;
    }

    int number_of_variables = count_unique_variables(expression);
    int number_of_rows = end_row - start_row + 1;
    int row_length = number_of_variables * 2 + strlen(expression) + 9;

    int64_t segment_length = (int64_t)number_of_rows * row_length;
    char *segment = (char *)malloc(segment_length + 1);

    if (segment == NULL)
    {
        fprintf(stderr, "Memory allocation for postfix segment failed in file %s at line %d\n", __FILE__, __LINE__);
        return (char *)NULL;
    }

    int added_rows = 0;
    // Generate the segment
    for (int i = start_row; i < end_row; i++)
    {
        char *row = generate_postfix_row(i, number_of_variables, expression);

        if (row == NULL)
        {
            fprintf(stderr, "Failed to generate row in file %s at line %d\n", __FILE__, __LINE__);
            free(segment);
            return (char *)NULL;
        }

        memcpy(segment + added_rows * row_length, row, row_length + 1);
        added_rows++;
        free(row);
    }
    segment[segment_length] = '\0';

    return segment;
}

char *generate_true_postfix_truth_table_segment(const char *expression, int start_row, int end_row)
{
    if (start_row < 0)
    {
        fprintf(stderr, "Failed to generate segment in file %s at line %d: invalid start row %d\n", __FILE__, __LINE__, start_row);
        return (char *)NULL;
    }
    if (end_row < 0)
    {
        fprintf(stderr, "Failed to generate segment in file %s at line %d: invalid end row %d\n", __FILE__, __LINE__, end_row);
        return (char *)NULL;
    }
    // Making sure not to overshoot the table
    if (end_row >= (1 << count_unique_variables(expression)))
    {
        end_row = (1 << count_unique_variables(expression));
    }
    int number_of_variables = count_unique_variables(expression);
    int number_of_rows = end_row - start_row;
    int row_length = number_of_variables * 2 + strlen(expression) + 9;
    int64_t segment_length = (int64_t)number_of_rows * row_length;
    char *segment = (char *)calloc(segment_length + 1, sizeof(char));

    if (segment == NULL)
    {
        fprintf(stderr, "Memory allocation for true postfix segment failed in file %s at line %d\n", __FILE__, __LINE__);
        return (char *)NULL;
    }

    int added_rows = 0;
    // Generate the segment
    for (int i = start_row; i < end_row; i++)
    {
        char *row = generate_postfix_row(i, number_of_variables, expression);
        if (row == NULL)
        {
            fprintf(stderr, "Failed to generate row in file %s at line %d\n", __FILE__, __LINE__);
            free(segment);
            return (char *)NULL;
        }
        if (row[row_length - 2] == '1')
        {
            memcpy(segment + (int64_t)added_rows * row_length, row, row_length);
            added_rows++;
        }
        free(row);
    }
    segment[segment_length] = '\0';

    return segment;
}

void *postfix_rows_generator(void *arg)
{

    postfix_thread_data *data = (postfix_thread_data *)arg;

    // Generate the segment data
    char *segment = generate_true_postfix_truth_table_segment(data->expression, data->start_row, data->end_row);
    if (segment == NULL)
    {
        fprintf(data->file, "Variables must be a-z lowercase.\nOperators are | OR; & AND; # XOR; > IMPLICATION; = IFF; - NOT\n");
        exit(EXIT_FAILURE);
    }

    if (data->start_row == 0 && data->end_row != data->start_row)
    {
        char *header = generate_header(data->expression);
        char *separator = generate_separator(data->expression);
        fprintf(data->file, "%s%s", header, separator);
        free(header);
        free(separator);
    }

    sem_wait(&data->semaphore[data->thread_id]);
    fprintf(data->file, "%s", segment);
    if (data->thread_id + 1 < data->num_threads)
    {
        sem_post(&data->semaphore[data->thread_id + 1]);
    }
    free(segment);

    pthread_exit(NULL);
}

void generate_postfix_table_body(const char *expression, FILE *file)
{
    int number_of_variables = count_unique_variables(expression);
    int number_of_rows = 1 << number_of_variables;

    // Allocating the maximum number of threads possible
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cores == -1)
    {
        perror("sysconf");
        exit(EXIT_FAILURE);
    }

    // Making as many threads as the machine has cores
    pthread_t threads[num_cores];
    postfix_thread_data thread_data[num_cores];
    sem_t semaphores[num_cores];

    // Initialize semaphores
    for (int i = 0; i < num_cores; i++)
    {
        if (sem_init(&semaphores[i], 0, 0) == -1)
        {
            perror("sem_init");
            exit(EXIT_FAILURE);
        }
    }
    sem_post(&semaphores[0]);

    // The factor to multiply the rows for each subsequent thread (e.g., 1.5)
    double factor = 1.5;

    // Calculate the sum of the geometric progression for the ratios
    double sum_of_ratios = ((1 / (factor - 1)) * (pow(factor, num_cores - 1) - 1)); // Sum of the geometric series

    // Calculate how many rows the first thread gets
    int first_thread_rows = number_of_rows / sum_of_ratios;

    int start_row = 0;

    // Distributing the workload across the threads
    for (int i = 0; i < num_cores; i++)
    {
        thread_data[i].file = file;
        thread_data[i].num_threads = num_cores;
        thread_data[i].thread_id = i;
        thread_data[i].semaphore = semaphores;
        thread_data[i].number_of_variables = number_of_variables;
        thread_data[i].expression = expression;
        thread_data[i].start_row = start_row;

        if (i == num_cores - 1)
        {
            // For the last thread, assign it the remaining rows
            thread_data[i].end_row = number_of_rows;
        }
        else
        {
            // Calculate the number of rows for this thread based on the factor
            int rows_for_current_thread = first_thread_rows * pow(factor, i);
            thread_data[i].end_row = start_row + rows_for_current_thread;
        }

        // Update the start row for the next thread
        start_row = thread_data[i].end_row;

        int rc = pthread_create(&threads[i], NULL, postfix_rows_generator, &thread_data[i]);
        if (rc)
        {
            fprintf(stderr, "Error creating thread %d: %d\n", i, rc);
            exit(EXIT_FAILURE);
        }
    }

    // Join all the threads
    for (int i = 0; i < num_cores; i++)
    {
        pthread_join(threads[i], NULL);
    }
}

char *generate_infix_row(int row_number, int number_of_variables, const char *expression, int *map, int expr_length)
{
    int row_length = number_of_variables * 2 + expr_length + 10; // Sufficient space for formatting
    char *row = (char *)malloc(row_length * sizeof(char));
    if (row == NULL)
    {
        fprintf(stderr, "Memory allocation failed for row in file %s at line %d\n", __FILE__, __LINE__);
        return (char *)NULL;
    }

    char *binary_number_string = int_to_binary_string(row_number, number_of_variables);
    if (binary_number_string == NULL)
    {
        fprintf(stderr, "Failed to create binary string in file %s at line %d\n", __FILE__, __LINE__);
        free(row);
        return (char *)NULL;
    }

    // Fill the row with binary values and spaces
    for (int j = 0; j < number_of_variables; j++)
    {
        row[2 * j] = binary_number_string[j];
        row[2 * j + 1] = ' '; // Add a space after each binary digit
    }
    row[number_of_variables * 2] = ':';  // Add the colon
    row[number_of_variables * 2 + 1] = ' '; // Add space after colon

    char *modified_expression = replace_with_binary(expression, row_number);
    if (modified_expression == NULL)
    {
        fprintf(stderr, "Failed to perform binary replacement in file %s at line %d\n", __FILE__, __LINE__);
        free(row);
        free(binary_number_string);
        return (char *)NULL;
    }

    char *rpn_expr = evaluate_expr(modified_expression);
    if (rpn_expr == NULL)
    {
        fprintf(stderr, "Failed to evaluate expression in file %s at line %d\n", __FILE__, __LINE__);
        free(row);
        free(binary_number_string);
        free(modified_expression);
        return (char *)NULL;
    }

    char *result = convert_evaled_rpn_to_infix(map, rpn_expr, expr_length);
    if (result == NULL)
    {
        fprintf(stderr, "Failed to convert to infix in file %s at line %d\n", __FILE__, __LINE__);
        free(row);
        free(binary_number_string);
        free(modified_expression);
        free(rpn_expr);
        return (char *)NULL;
    }

    // Position to insert the result into the row
    int position = number_of_variables * 2 + 2; 
    // Handle the case of a single variable expression
    if (strlen(modified_expression) == 1)
    {
        row[position++] = ' ';
        row[position++] = ' ';
        row[position++] = ':';
        row[position++] = ' '; // Space after ":"
        row[position++] = ' ';
        row[position++] = ' '; 
        row[position++] = modified_expression[0]; // The single variable result
        row[position++] = '\n'; // New line at the end
    }
    // Handle the general case
    else
    {
        int rpn_length = strlen(rpn_expr);
        int res_len = strlen(result);
        // Copy the result into the row
        memcpy(row + position, result, res_len);
        position += res_len;
        row[position++] = ' '; // Space before the final result
        row[position++] = ':'; // Add the colon
        row[position++] = ' '; // Space after colon
        row[position++] = ' ';
        row[position++] = ' ';

        // Add the last character of rpn_expr
        row[position++] = rpn_expr[rpn_length - 1];
        row[position++] = '\n'; // New line at the end
    }

    // Null-terminate the string
    row[position] = '\0';

    // Clean up dynamically allocated memory
    free(result);
    free(modified_expression);
    free(binary_number_string);
    free(rpn_expr);

    return row;
}

char *generate_infix_truth_table_segment(const char *expression, int start_row, int end_row)
{

    // Making sure not to overshoot the table
    if (end_row > (1 << count_unique_variables(expression)))
    {
        end_row = (1 << count_unique_variables(expression));
    }

    int expression_length = strlen(expression);
    int *rpnArr = infix_map(expression);
    if (rpnArr == NULL)
    {
        fprintf(stderr, "Failed to generate infix map in file %s at line %d\n", __FILE__, __LINE__);
        return (char *)NULL;
    }
    char *rpn_expression = shunting_yard(expression);
    if (rpn_expression == NULL)
    {
        fprintf(stderr, "Failed to convert infix expression in file %s at line %d\n", __FILE__, __LINE__);
        free(rpnArr);
        return (char *)NULL;
    }
    int number_of_variables = count_unique_variables(expression);
    int number_of_rows = end_row - start_row + 1;
    int row_length = number_of_variables * 2 + strlen(expression) + 9;
    int64_t segment_length = (int64_t)number_of_rows * row_length;

    char *segment = (char *)calloc(segment_length + 1, sizeof(char));

    if (segment == NULL)
    {
        fprintf(stderr, "Memory allocation for segment failed in file %s at line %d\n", __FILE__, __LINE__);
        free(rpnArr);
        free(rpn_expression);
        return (char *)NULL;
    }
    int added_rows = 0;
    // Generate the segment data
    for (int i = start_row; i < end_row; i++)
    {
        // Get the length of the row
        char *row = generate_infix_row(i, number_of_variables, rpn_expression, rpnArr, expression_length);
        if (row == NULL)
        {
            fprintf(stderr, "Failed to generate row in file %s at line %d\n", __FILE__, __LINE__);
            free(rpnArr);
            free(rpn_expression);
            free(segment);
            return (char *)NULL;
        }
        memcpy(segment + added_rows * row_length, row, row_length);
        added_rows++;
        free(row);
    }

    segment[segment_length] = '\0';
    free(rpn_expression);
    free(rpnArr);
    // Ensure the segment is null-terminated if needed
    return segment;
}

char *generate_true_infix_truth_table_segment(const char *expression, int start_row, int end_row)
{
    // Making sure not to overshoot the table
    if (end_row >= (1 << count_unique_variables(expression)))
    {
        end_row = (1 << count_unique_variables(expression));
    }

    int expression_length = strlen(expression);
    int *rpnArr = infix_map(expression);
    if (rpnArr == NULL)
    {
        fprintf(stderr, "Failed to generate infix map in file %s at line %d\n", __FILE__, __LINE__);
        return (char *)NULL;
    }
    char *rpn_expression = shunting_yard(expression);
    if (rpn_expression == NULL)
    {
        fprintf(stderr, "Failed to convert infix expression in file %s at line %d\n", __FILE__, __LINE__);
        free(rpnArr);
        return (char *)NULL;
    }
    // Debug
    // printf("start row: %ld end row: %ld\n", start_row, end_row);
    int number_of_variables = count_unique_variables(expression);
    int number_of_rows = end_row - start_row;
    int row_length = number_of_variables * 2 + strlen(expression) + 9;
    int64_t segment_length = (int64_t)number_of_rows * row_length + 1;
    int added_rows = 0;
    // Allocating an upper bound for the true rows
    // Using calloc as I might not use the entire allocated memory space
    // So want to make sure its not full of garbage
    char *segment = (char *)calloc(segment_length + 1, sizeof(char));
    if (segment == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return (char *)NULL;
    }

    for (int i = start_row; i < end_row; i++)
    {
        char *row = generate_infix_row(i, number_of_variables, rpn_expression, rpnArr, expression_length);
        if (row == NULL)
        {
            fprintf(stderr, "Failed to generate row in file %s at line %d\n", __FILE__, __LINE__);
            free(rpnArr);
            free(rpn_expression);
            free(segment);
            return (char *)NULL;
        }
        if (row[row_length - 2] == '1')
        {
            memcpy(segment + (int64_t)added_rows * row_length, row, row_length);
            added_rows++;
        }
        free(row);
    }

    segment[segment_length] = '\0';
    free(rpn_expression);
    free(rpnArr);

    return segment;
}

void *infix_rows_generator(void *arg)
{
    infix_thread_data *data = (infix_thread_data *)arg;

    // Generate the segment data
    char *segment = generate_true_infix_truth_table_segment(data->expression, data->start_row, data->end_row);
    if (segment == NULL)
    {
        fprintf(data->file, "Variables must be a-z lowercase.\nOperators are | OR; & AND; # XOR; > IMPLICATION; = IFF; - NOT\n");
        exit(EXIT_FAILURE);
    }
    if (data->start_row == 0 && data->end_row != data->start_row)
    {
        char *header = generate_header(data->expression);
        char *separator = generate_separator(data->expression);
        fprintf(data->file, "%s%s", header, separator);
        free(header);
        free(separator);
    }
    sem_wait(&data->semaphore[data->thread_id]);

    fprintf(data->file, "%s", segment);
    if (data->thread_id + 1 < data->num_threads)
    {
        sem_post(&data->semaphore[data->thread_id + 1]);
    }
    free(segment);
    pthread_exit(NULL);
}

void generate_infix_table_body(const char *expression, FILE *file)
{
    int *inf_map = infix_map(expression);
    int number_of_variables = count_unique_variables(expression);
    int number_of_rows = 1 << number_of_variables;

    // Get the number of cores (threads)
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cores == -1)
    {
        perror("sysconf");
        exit(EXIT_FAILURE);
    }

    // Thread array and thread data array
    pthread_t threads[num_cores];
    infix_thread_data thread_data[num_cores];
    sem_t semaphores[num_cores];

    for (int i = 0; i < num_cores; i++)
    {
        if (sem_init(&semaphores[i], 0, 0) == -1)
        {
            perror("sem_init");
            exit(EXIT_FAILURE);
        }
    }

    // Post the first semaphore to allow the first thread to start immediately
    sem_post(&semaphores[0]);

    // The factor to multiply the rows for each subsequent thread (e.g., 1.5)
    // Computing rows like this allows quicker freeing of memory reducing RAM demand.
    double factor = 1.5;

    // Use formula for sum of geometric series
    double sum_of_ratios = ((1 / (factor - 1)) * (pow(factor, num_cores - 1) - 1));

    // Calculate how many rows the first thread gets
    // In the geometric progression this would be 'a' where the progression is
    // a + ar + ar^2 + ... + ar^(number_of_cores - 1)
    int first_thread_rows = number_of_rows / sum_of_ratios;
    int start_row = 0;

    for (int i = 0; i < num_cores; i++)
    {
        // Assign rows based on the logic: each thread gets factor times the rows of the previous thread
        if (i == num_cores - 1)
        {
            // The last thread takes all the remaining rows
            thread_data[i].end_row = number_of_rows;
        }
        else
        {
            // Calculate the rows for the current thread as first_thread_rows * (factor^i)
            int rows_for_current_thread = first_thread_rows * pow(factor, i);
            thread_data[i].end_row = start_row + rows_for_current_thread;
        }
        thread_data[i].file = file;
        thread_data[i].semaphore = semaphores;
        thread_data[i].num_threads = num_cores;
        thread_data[i].thread_id = i;
        thread_data[i].expression_length = strlen(expression);
        thread_data[i].map = inf_map;
        thread_data[i].number_of_variables = number_of_variables;
        thread_data[i].expression = expression;
        thread_data[i].start_row = start_row;

        // Update start_row for the next thread
        start_row = thread_data[i].end_row;
        int rc = pthread_create(&threads[i], NULL, infix_rows_generator, &thread_data[i]);
        if (rc)
        {
            fprintf(stderr, "Error creating thread %d: %d\n", i, rc);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_cores; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Free allocated memory
    free(inf_map);
}

char *generate_header(const char *expression)
{
    // To track the order of unique lowercase letters
    char unique_vars[26] = {0};
    bool present[26] = {0};
    int num_vars = 0;

    for (int i = 0; expression[i] != '\0'; i++)
    {
        char ch = expression[i];
        if (islower(ch) && !present[ch - 'a'])
        {
            unique_vars[num_vars++] = ch;
            present[ch - 'a'] = true;
        }
    }
    // Calculate header length
    int header_length = num_vars * 2 + strlen(expression) + 13; // 12 for ":  : Result\n" + 1 for null terminator
    char *header = (char *)malloc((header_length + 1) * sizeof(char));
    if (header == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    int offset = 0;
    for (int i = 0; i < num_vars; i++)
    {
        snprintf(header + offset, header_length - offset, "%c ", unique_vars[i]);
        offset += 2; // Move the offset by 2 characters (variable + space)
    }
    snprintf(header + offset, header_length - offset, ": %s : Result\n", expression);
    return header;
}

char *generate_separator(const char *expression)
{
    int number_of_variables = count_unique_variables(expression);
    int separator_length = (number_of_variables * 2 + strlen(expression) + 13);
    char *separator = (char *)malloc(separator_length * sizeof(char));
    if (separator == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < separator_length - 2; i++)
    {
        snprintf(separator + i, separator_length - i, "%c", '=');
    }
    snprintf(separator + strlen(separator), 2, "%c", '\n');
    return separator;
}
