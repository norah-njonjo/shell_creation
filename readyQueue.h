#ifndef READYQUEUE_H
#define READYQUEUE_H
#include "pcb.h"

typedef struct ReadyQueue {
    PCB* head;
    PCB* tail;
    int size;
} ReadyQueue;

ReadyQueue* rq_create(void);
void rq_destroy(ReadyQueue* q);
void rq_enqueue(ReadyQueue* q, PCB* p, int agesort);
PCB* rq_dequeue(ReadyQueue* q);
int is_empty(ReadyQueue* q);

#endif
