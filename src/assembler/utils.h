#ifndef UTILS_H
#define UTILS_H

#define MAX_LINE_LENGTH 1024

#include <stddef.h>

char **read_file_to_lines(const char *filename, int* num_lines);
void trim_whitespace(char *str);

void append_bytes_to_file(const char *filename, const void *data, size_t size);

#endif
