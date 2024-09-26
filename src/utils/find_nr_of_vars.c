#include <stdbool.h>
#include <ctype.h> 

int count_unique_variables(const char *expression) {
    bool present[26] = {0}; // To track which letters have been seen
    int count = 0;

    for (int i = 0; expression[i] != '\0'; i++) {
        char ch = expression[i];
        if (islower(ch) && !present[ch - 'a']) {
            present[ch - 'a'] = true;
            count++;
        }
    }

    return count;
}

