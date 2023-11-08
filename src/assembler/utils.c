#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char** read_file_to_lines(const char* filename, int* num_lines) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open the file");
        return NULL;
    }

    char** lines = NULL;
    char buffer[MAX_LINE_LENGTH];
    int line_count = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Remove the newline character at the end of the line, if present
        size_t line_length = strlen(buffer);
        if (line_length > 0 && buffer[line_length - 1] == '\n') {
            buffer[line_length - 1] = '\0';
        }

        // Allocate memory for the line and copy it into the array
        char* line_copy = strdup(buffer);
        if (line_copy == NULL) {
            perror("Memory allocation failed");
            fclose(file);
            // Free previously allocated memory
            for (int i = 0; i < line_count; i++) {
                free(lines[i]);
            }
            free(lines);
            return NULL;
        }

        // Reallocate the lines array to hold the new line
        char** new_lines = realloc(lines, (line_count + 1) * sizeof(char*));
        if (new_lines == NULL) {
            perror("Memory allocation failed");
            fclose(file);
            free(line_copy);
            // Free previously allocated memory
            for (int i = 0; i < line_count; i++) {
                free(lines[i]);
            }
            free(lines);
            return NULL;
        }

        lines = new_lines;
        lines[line_count] = line_copy;
        line_count++;
    }

    fclose(file);

    *num_lines = line_count;
    return lines;
}
