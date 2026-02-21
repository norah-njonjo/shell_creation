
#ifndef PCB_H
#define PCB_H

typedef struct PCB {
    int pid;
    int start;
    int length;
    int age;    // job length score for SJF with aging
    int pc;
    struct PCB* next;
} PCB;

PCB* create_pcb(int start, int length);
int compare_job_length(const void* a, const void* b);
void free_pcb(PCB* pcb);

#endif