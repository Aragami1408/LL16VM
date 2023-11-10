#ifndef LABEL_EVALUATOR_H
#define LABEL_EVALUATOR_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../base_types.h"
#include "mpc.h"

#define MAX_LABEL_LENGTH 50
#define TABLE_SIZE 100

typedef struct {
	char name[MAX_LABEL_LENGTH + 1];	
	int address;
} label_t;

typedef struct {
	label_t data[TABLE_SIZE];
} label_hashmap_t;

void init_label_hashmap(label_hashmap_t *hashmap);
bool insert_label(label_hashmap_t *hashmap, const char *name, int address);
bool find_label(const label_hashmap_t *hashmap, const char *name, int *address);
bool remove_label(label_hashmap_t *hashmap, const char *name);
void print_label_hashmap(const label_hashmap_t *hashmap);

#endif
