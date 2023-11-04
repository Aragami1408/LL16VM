#ifndef ASSEMBLER_STR_UTILS_H
#define ASSEMBLER_STR_UTILS_H

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

void trim_whitespace(char *str);

char *str_replace(char *orig, char *rep, char *with);

#endif
