#ifndef UTILS_H
#define UTILS_H

#define MAX_LINE_LENGTH 1024

char **read_file_to_lines(const char *filename, int* num_lines);

void trim_whitespace(char *str);

#endif
