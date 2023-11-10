#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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

void trim_whitespace(char* str) {
    if (str == NULL) {
        return; // Handle NULL strings if needed
    }

    int length = (int) strlen(str);

    // Trim leading whitespace
    int start = 0;
    while (start < length && isspace((unsigned char)str[start])) {
        start++;
    }

    // Trim trailing whitespace
    int end = length - 1;
    while (end > start && isspace((unsigned char)str[end])) {
        end--;
    }

    // Shift the non-whitespace characters to the beginning of the string
    int i, j;
    for (i = start, j = 0; i <= end; i++, j++) {
        str[j] = str[i];
    }

    // Null-terminate the trimmed string
    str[j] = '\0';
}

void append_bytes_to_file(const char *filename, const void *data, size_t size) {
    FILE *file = fopen(filename, "ab"); // "ab" mode for binary append

    if (file == NULL) {
        perror("Error opening file for append");
        exit(EXIT_FAILURE);
    }

    size_t elements_written = fwrite(data, 1, size, file);

    if (elements_written != size) {
        perror("Error writing to file");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}
