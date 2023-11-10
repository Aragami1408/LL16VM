#include "label_evaluator.h"

static unsigned int hash(const char *name) {
    unsigned int hash = 0;
    while (*name) {
        hash = (hash * 31) + (*name);
        name++;
    }
    return hash % TABLE_SIZE;
}

void init_label_hashmap(label_hashmap_t *hashmap) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        memset(hashmap->data[i].name, 0, MAX_LABEL_LENGTH + 1);
        hashmap->data[i].address = -1;
    }
}

bool insert_label(label_hashmap_t *hashmap, const char *name, int address) {
    unsigned int index = hash(name);
    label_t *label = &hashmap->data[index];

    if (strlen(name) > MAX_LABEL_LENGTH) {
        return false;  // Label name is too long
    }

    if (label->address != -1) {
        return false;  // Label already exists
    }

    strncpy(label->name, name, MAX_LABEL_LENGTH);
    label->name[MAX_LABEL_LENGTH] = '\0'; // Ensure null-termination
    label->address = address;

    return true;
}

bool find_label(const label_hashmap_t *hashmap, const char *name, int *address) {
    unsigned int index = hash(name);
    const label_t *label = &hashmap->data[index];

    if (label->address == -1) {
        return false;  // Label not found
    }

    if (strcmp(label->name, name) == 0) {
        *address = label->address;
        return true;  // Label found
    }

    return false;  // Label not found
}

bool remove_label(label_hashmap_t *hashmap, const char *name) {
    unsigned int index = hash(name);
    label_t *label = &hashmap->data[index];

    if (label->address == -1) {
        return false;  // Label not found
    }

    if (strcmp(label->name, name) == 0) {
        memset(label->name, 0, MAX_LABEL_LENGTH + 1);
        label->address = -1;
        return true;  // Label removed
    }

    return false;  // Label not found
}

void print_label_hashmap(const label_hashmap_t *hashmap) {
	printf("Label Map: \n");
	for (int i = 0; i < TABLE_SIZE; i++) {
		if (hashmap->data[i].address != -1) {
			printf("\tIndex: %d, Name: %s, Address: $%04x\n", i, hashmap->data[i].name, hashmap->data[i].address);
		}
	}
}
