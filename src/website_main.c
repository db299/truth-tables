#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "table_builders_for_webpage/table_builders.h"
#include "converters/shunting_yard.h"
#include "utils/find_nr_of_vars.h"

int main(int argc, char *argv[])
{

    if (argc > 4)
    {
        printf("Usage: %s <expression> <file_name> for writing to a file\nvalid variables: a-z\nvalid operators: not −; or |; and &; xor #; implication >; equivalence =; brackets for infix ();\n", argv[0]);
        printf("For generating segments, use %s <expression> <start> <end>\n", argv[0]);
        return 1; // Exit with an error code
    }
    const char *expression = argv[1];

    // Case where binary is being called to generate a full table
    if (argc == 3)
    {

        char *file_name = argv[2];
        FILE *file = fopen(file_name, "w");
        if (is_valid_infix(expression))
        {
            generate_infix_table_body(expression, file);
        }
        else
        {
            generate_postfix_table_body(expression, file);
        }
        if (fclose(file) != 0)
        {
            perror("Error closing file");
            return 1; // Exit if file closing fails
        }
    }
    else if (argc == 4)
    {
        long start = strtol(argv[2], NULL, 10);
        long end = strtol(argv[3], NULL, 10);

        if (start >= 1 << count_unique_variables(expression))
        {
            exit(EXIT_FAILURE);
        }
        if (is_valid_infix(expression))
        {
            char *header = generate_header(expression);
            char *separator = generate_separator(expression);
            char *segment = generate_infix_truth_table_segment(expression, start, end);

            if (segment == NULL)
            {
                printf("Variables must be a-z lowercase.\nOperators are | OR; & AND; # XOR; > IMPLICATION; = IFF; - NOT\n");
                exit(EXIT_FAILURE);
            }

            if (start == 0 && start != end)
            {
                printf("%s", header);
                printf("%s", separator);
                free(header);
                free(separator);
            }

            printf("%s", segment);
            free(segment);
        }
        else
        {
            char *header = generate_header(expression);
            char *separator = generate_separator(expression);
            char *segment = generate_postfix_truth_table_segment(expression, start, end);
            if (segment == NULL)
            {
                printf("Variables must be a-z lowercase.\nOperators are | OR; & AND; # XOR; > IMPLICATION; = IFF; - NOT\n");
                exit(EXIT_FAILURE);
            }
            if (start == 0 && start != end)
            {
                printf("%s", header);
                printf("%s", separator);
                free(header);
                free(separator);
            }
            printf("%s", segment);
            free(segment);
        }
    }
    return 0;
}