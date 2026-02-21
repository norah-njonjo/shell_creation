#include <stdlib.h>
#include "readyQueue.h"
#include "pcb.h"

//create a ReadyQueue
ReadyQueue* rq_create(void) {
    ReadyQueue* q = malloc(sizeof(*q));
    if (!q) return NULL;
    q->head = q->tail = NULL;
    q->size = 0;
    return q;
}


//destroy a ReadyQueue
void rq_destroy(ReadyQueue* q) {
    free(q);
}

//adds a PCB to the end of the queue
void rq_enqueue(ReadyQueue* q, PCB* p, int agesort) {
    /*
    p->next = NULL;
    if (q->tail == NULL || (q->size==0)) {
        q->head = q->tail = p;
    } else {
        if (!agesort) {
            q->tail->next = p;
            q->tail = p;
        } else {
            int pAge = p->age;
            PCB* tmp = q->head;
            // if p is still youngest, put it at head
            if (pAge<=tmp->age && pAge>0) {
                q->head = p;
                p->next = tmp;
            }
            // p is not youngest anymore
            int inserted = 0;
            while (tmp->next != NULL) {
                if (pAge < tmp->next->age) { //if the next one is older, then insert it here
                    p->next = tmp->next;
                    tmp->next = p;
                    inserted = 1;
                    break;
                }
                tmp = tmp->next;
            }
            if (!inserted) { // then tmp is the tail, so p is oldest
                tmp->next = p;
                q->tail = p;
                p->next = NULL;
            }
        }
        
    }
    q->size++;
    */

    if (!q || !p) return;

    p->next = NULL;                 // clear stale pointer

    // empty queue
    if (q->head == NULL) {
        q->head = q->tail = p;
        q->size = 1;
        return;
    }

    // plain append for non-aging policies
    if (!agesort) {
        q->tail->next = p;
        q->tail = p;
        q->size++;
        return;
    }

    // agesort == true: stable insert so equals keep FIFO order
    PCB *prev = NULL;
    PCB *cur  = q->head;

    // Advance past nodes with age <= p->age, stop at first cur->age > p->age
    while (cur && cur->age <= p->age) {
        prev = cur;
        cur = cur->next;
    }

    if (prev == NULL) {
        // insert at head (all existing nodes have age > p->age)
        p->next = q->head;
        q->head = p;
    } else {
        // insert after prev and before cur (cur may be NULL -> append at tail)
        prev->next = p;
        p->next = cur;
        if (cur == NULL) q->tail = p;
    }

    q->size++;
}

//removes and returns the PCB at the front of queue
PCB* rq_dequeue(ReadyQueue* q) {
    if (q->head == NULL) return NULL;
    PCB* p = q->head;
    q->head = q->head->next;
    if (q->head == NULL) q->tail = NULL;
    p->next = NULL;
    q->size--;
    return p;
}

// checks if the queue is empty
int is_empty(ReadyQueue* q) {
    return q->head == NULL;
}