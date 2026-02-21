#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "readyQueue.h"   // defines ReadyQueue, enqueue/dequeue/is_empty
#include "pcb.h"          // defines PCB type


static int run_pcb_lines(PCB *p, int lines_to_run);
void scheduler_run(ReadyQueue *rq, const char *policy_str);

#endif