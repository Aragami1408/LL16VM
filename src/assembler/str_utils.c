#include "str_utils.h"

void trim_whitespace(char *str) {
    int start = 0, end = strlen(str) - 1;

    // Trim leading whitespace
    while (isspace(str[start])) {
	start++;
    }	

    // If the string is all whitespace, exit early
    if (start >= end) {
	str[0] = '\0';
	return;
    }

    // Trim trailing whitespace
    while (isspace(str[end])) {
	end--;
    }

    int length = end - start + 1;
    memmove(str, str + start, length);

    // Null-terminate the new string
    str[length] = '\0';
}
