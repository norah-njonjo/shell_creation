#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "shellmemory.h"


struct memory_struct {
    char *var;
    char *value;
};

struct memory_struct shellmemory[MEM_SIZE];
char* program_lines[MEM_SIZE];  // Array to store up to 1000 lines of code
int next_free_line = 0;    // Tracks where to store the next line

// Helper functions
int match(char *model, char *var) {
    int i, len = strlen(var), matchCount = 0;
    for (i = 0; i < len; i++) {
        if (model[i] == var[i])
            matchCount++;
    }
    if (matchCount == len) {
        return 1;
    } else
        return 0;
}

// Shell memory functions

void mem_init() {
    int i;
    for (i = 0; i < MEM_SIZE; i++) {
        shellmemory[i].var = "none";
        shellmemory[i].value = "none";
    }
}

// Set key value pair
void mem_set_value(char *var_in, char *value_in) {
    int i;

    for (i = 0; i < MEM_SIZE; i++) {
        if (strcmp(shellmemory[i].var, var_in) == 0) {
            shellmemory[i].value = strdup(value_in);
            return;
        }
    }

    //Value does not exist, need to find a free spot.
    for (i = 0; i < MEM_SIZE; i++) {
        if (strcmp(shellmemory[i].var, "none") == 0) {
            shellmemory[i].var = strdup(var_in);
            shellmemory[i].value = strdup(value_in);
            return;
        }
    }

    return;
}

//get value based on input key
char *mem_get_value(char *var_in) {
    int i;

    for (i = 0; i < MEM_SIZE; i++) {
        if (strcmp(shellmemory[i].var, var_in) == 0) {
            return strdup(shellmemory[i].value);
        }
    }
    return "Variable does not exist";
}

char* mem_get_program_line(int index) {
    if (index < 0 || index >= next_free_line) {
        return NULL;  // Invalid index
    }
    return program_lines[index];
}

void mem_free_program_lines(int start, int length) {
    int end = start + length - 1;   // last valid index for this block
    for (int i = start; i <= end; i++) {
        if (program_lines[i] != NULL) {
            free(program_lines[i]);  // Free the strdup'd string
            program_lines[i] = NULL;
        }
    }
}

int mem_load_script(char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) return -1;

    char line[100];
    int start = next_free_line;
    int count = 0;

    while (fgets(line, 100, fp) != NULL) {
        // Remove newline character
        line[strcspn(line, "\n")] = '\0';

        program_lines[next_free_line] = strdup(line);
        next_free_line++;
        count++;
    }

    fclose(fp);
    return count; // return number of lines loaded
}



