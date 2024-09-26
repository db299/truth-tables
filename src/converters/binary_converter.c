#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "../utils/find_nr_of_vars.h"

#include "binary_converter.h"

char *int_to_binary_string(int number, int length)
{

    if (length < 0)
    {
        fprintf(stderr, "Length must be greater than or equal 0.\n");
        return (char *)NULL;
    }
    if (number < 0 || number >= (1 << length))
    {
        fprintf(stderr, "Binary representation of number %d can't fit inside %d characters.\n", number, length);
        return (char *)NULL;
    }
    char *binary_string = (char *)malloc((length + 1) * sizeof(char));
    if (binary_string == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for binary string in %s at line %d.\n", __FILE__, __LINE__);
        return (char *)NULL;
    }
    // Start from the end of binary string
    char *ptr = binary_string + length;
    // Make sure its null terminated
    *ptr = '\0';
    // Loop through each bit from right to left
    while (length--)
    {
        // write the string by writing at each address inside it
        *(--ptr) = (number & 1) + '0';
        // Right shift the number to get the next bit
        number >>= 1;
    }
    return binary_string;
}

char *replace_with_binary(const char *original_expression, int number)
{
    if (number < 0)
    {
        fprintf(stderr, "Cannot interpret negative number association to row in truth table in %s at line %d.\n", __FILE__, __LINE__);
        return (char *)NULL;
    }
    // Track unique lowercase letters in the expression
    char unique_vars[26] = {0}; // To store unique variables found
    int unique_count = 0;
    for (int i = 0; original_expression[i] != '\0'; i++)
    {
        char ch = original_expression[i];
        // Check that ch is lowercase and doesnt exist inside uniquevars
        if (islower(ch) && !strchr(unique_vars, ch))
        {
            unique_vars[unique_count++] = ch;
        }
    }
    // Ensure the number of unique variables does not exceed 26
    // Should never happen but just in case
    if (unique_count > 26)
    {
        fprintf(stderr, "Too many unique variables (more than 26).\n");
        return (char *)NULL;
    }
    // Generate a binary string for the number of variables
    char *binary_string = int_to_binary_string(number, unique_count);
    if (binary_string == NULL)
    {
        fprintf(stderr, "Failed to create binary string in file %s at line %d\n", __FILE__, __LINE__);
        return (char *)NULL;
    }
    // Create a mapping from variables to their corresponding binary values
    char replacement_map[26] = {0}; // Map for all lowercase letters
    for (int i = 0; i < unique_count; i++)
    {
        // Each character is mapped based on its alphabetic position
        replacement_map[unique_vars[i] - 'a'] = binary_string[i];
    }
    // Create a copy of the original expression to avoid modifying it directly
    int len = strlen(original_expression);
    char *modified_expression = (char *)malloc((len + 1) * sizeof(char));
    if (modified_expression == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for modified expression in %s at line %d\n", __FILE__, __LINE__);
        free(binary_string);
        return (char *)NULL;
    }
    memcpy(modified_expression, original_expression, len + 1);
    // Replace lowercase alphabetic characters in the  with binary equivalents
    for (int i = 0; i < len; i++)
    {
        char current_char = modified_expression[i];

        int index = current_char - 'a'; // Determine index in the map
        // Ensure index is within valid range
        if (index >= 0 && index < 26)
        {
            modified_expression[i] = replacement_map[index];
        }
    }

    free(binary_string);
    return modified_expression;
}

char *convert_evaled_rpn_to_infix(const int *map, const char *rpn, int map_size)
{
    if (map_size < 0)
    {
        fprintf(stderr, "Invalid size for infix map in %s at line %d\n", __FILE__, __LINE__);
        return (char *)NULL;
    }

    // Check for duplicates
    int *seen = calloc(map_size, sizeof(int));
    if (seen == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for seen array in %s at line %d\n", __FILE__, __LINE__);
        return (char *)NULL;
    }

    for (int i = 0; i < map_size; i++)
    {
        if (map[i] != -1)
        {
            if (map[i] < 0 || map[i] >= map_size)
            {
                fprintf(stderr, "Index out of bounds in map at position %d\n", i);
                free(seen);
                return (char *)NULL;
            }

            if (seen[map[i]] != 0)
            {
                fprintf(stderr, "Duplicate index %d found in map at position %d\n", map[i], i);
                free(seen);
                return (char *)NULL;
            }

            seen[map[i]] = 1;
        }
    }

    // Free the seen array after checking for duplicates
    free(seen);

    // Allocate memory for the infix expression based on map_size
    char *infix = malloc((map_size + 1) * sizeof(char));
    if (infix == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for infix expression in %s at line %d\n", __FILE__, __LINE__);
        return (char *)NULL;
    }

    // Initialize the infix array with spaces to denote empty positions
    memset(infix, ' ', map_size);
    infix[map_size] = '\0';

    // Populate the infix array using the map array
    for (int i = 0; i < map_size; i++)
    {
        if (map[i] != -1)
        {
            infix[map[i]] = rpn[i];
        }
    }

    return infix;
}