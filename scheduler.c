#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scheduler.h"
#include "shell.h"       // provides extern ReadyQueue *global_rq and parseInput prototype
#include "shellmemory.h" // get_program_line / get_program_length OR direct access to program_lines




void scheduler_run(ReadyQueue *rq, const char *policy_str) {
    if (!rq) {
        fprintf(stderr, "scheduler_run: null ready queue\n");
        return;
    }


    int is_rr = (policy_str && strcmp(policy_str, "RR") == 0);
    int is_rr30 = (policy_str && strcmp(policy_str, "RR30") == 0);
    int is_sjf = (policy_str && strcmp(policy_str, "SJF") == 0);
    int is_aging = (policy_str && strcmp(policy_str, "AGING") == 0);
    int is_fcfs = (policy_str && strcmp(policy_str, "FCFS") == 0);


    if (is_rr || is_rr30) {
        int quantum;
        if (is_rr) quantum = 2;
        else quantum = 30;

        while (!is_empty(rq)) {
            PCB *current = rq_dequeue(rq);
            if (current == NULL) break;

            int instr_count = 0;

            /* execute up to 2 instructions at a time */
            while (current->pc < current->length && instr_count < quantum) {
                int abs_idx = current->start + current->pc;
                char *instruction = mem_get_program_line(abs_idx);

                /*if line missing, advance pc to skip*/
                if (instruction == NULL) {
                    current->pc++;
                    instr_count++;
                    continue;
                }

                int rc = parseInput(instruction);
                current->pc++;
                instr_count++;

                if (rc != 0) {
                   printf("Parseinput failed\n");
                   return;
                }
            }

            if (current->pc >= current->length) {
                /* then process has finished: free program text and PCB */
                mem_free_program_lines(current->start, current->length);
                free_pcb(current);
            } else {
                /* quantum time expired but process not finished: re-enqueue at tail */
                rq_enqueue(rq, current,0);
            }
        }

    } else if (is_aging) {

        while (!is_empty(rq)) {
            PCB *current = rq_dequeue(rq);
            if (current == NULL) break;

            int instr_count = 0;

            /* execute up to 2 instructions at a time */
            while (current->pc < current->length && instr_count < 1) {
                int abs_idx = current->start + current->pc;
                char *instruction = mem_get_program_line(abs_idx);

                /*if line missing, advance pc to skip*/
                if (instruction == NULL) {
                    current->pc++;
                    instr_count++;
                    continue;
                }

                int rc = parseInput(instruction);
                current->pc++;
                instr_count++;

                if (rc != 0) {
                   printf("Parseinput failed\n");
                   return;
                }
            }

            if (current->pc >= current->length) {
                /* then process has finished: free program text and PCB */
                mem_free_program_lines(current->start, current->length);
                free_pcb(current);
            } else {
                // quantum time expired but process not finished

                // de-age all other processes
                //PCB *rqPcb = rq->head; // the pointer to pcbs in queue
                for (PCB *tmp = rq->head; tmp != NULL; tmp = tmp->next) {
                    if (tmp->age > 0) tmp->age--;
                }
                rq_enqueue(rq, current, 1);
            }
        }

    } else if (is_fcfs || is_sjf) { // Non-preemptive: FCFS or SJF (if SJF, already sorted)

        while (!is_empty(rq)) {
            PCB* current = rq_dequeue(rq);
            if (current == NULL) break;

            // Run to completion
            while (current->pc < current->length) {
                int abs_idx = current->start + current->pc;
                char *instruction = mem_get_program_line(abs_idx); // or program_lines[abs_idx]
                if (instruction == NULL) {
                  // Defensive: if a line is missing, skip it but advance pc to avoid infinite loop
                    current->pc++;
                    continue;
                }

                int rc = parseInput(instruction); // capture interpreter return
                current->pc++;
                if (rc != 0) {
                    printf("Parseinput failed\n");
                    return;
                }
            }

            // finished: free program text and the PCB
            mem_free_program_lines(current->start, current->length);
            free_pcb(current);
        }
    } else {
        perror("Wrong scheduling policy\n");
        return;
    }



}

