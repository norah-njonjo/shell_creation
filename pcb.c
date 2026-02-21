// pcb.c
#include <stdlib.h>
#include "pcb.h"

static int next_pid = 1;

PCB* create_pcb(int start, int length) {
    PCB* new_pcb = malloc(sizeof(PCB));
    new_pcb->pid = next_pid++;
    new_pcb->start = start;
    new_pcb->length = length;
    new_pcb->age = length;
    new_pcb->pc = 0;
    new_pcb->next = NULL;
    return new_pcb;
}

// Comparison function for qsort
int compare_job_length(const void* a, const void* b) {
    PCB* pcb_a = *(PCB**)a;
    PCB* pcb_b = *(PCB**)b;
    return pcb_a->length - pcb_b->length;
}

void free_pcb(PCB* pcb) {
    if (pcb != NULL) {
        free(pcb);
    }
}