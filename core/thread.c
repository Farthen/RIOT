/**
 * thread functions
 *
 * Copyright (C) 2010 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @ingroup kernel
 * @{
 * @file
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 * @}
 */

#include <errno.h>
#include <malloc.h>
#include <stdio.h>

#include "thread.h"
#include "kernel.h"

//#define ENABLE_DEBUG
#include "debug.h"
#include "kernel_intern.h"
#include "bitarithm.h"
#include "hwtimer.h"
#include "scheduler.h"

inline int thread_getpid() {
    return fk_thread->pid;
}

unsigned int thread_getstatus(int pid) {
    if (fk_threads[pid]==NULL)
        return STATUS_NOT_FOUND;
    return fk_threads[pid]->status;
}

void thread_sleep() {
    if ( inISR()) return;
    dINT();
    sched_set_status((tcb*)fk_thread, STATUS_SLEEPING);
    fk_yield();
}

int thread_wakeup(int pid) {
    int isr = inISR();
    if (! isr) dINT();

    int result = fk_threads[pid]->status;
    if (result == STATUS_SLEEPING) {
        sched_set_status((tcb*)fk_threads[pid], STATUS_RUNNING);
        if (!isr) {
            eINT();
            fk_yield();
        } else {
            fk_context_switch_request = 1;
        }
        return 0;
    } else {
        if (!isr) eINT();
        return STATUS_NOT_FOUND;
    }
}

int fk_measure_stack_free(char* stack) {
    unsigned int* stackp = (unsigned int*)stack;
    /* assumption that the comparison fails before or after end of stack */
    while( *stackp == (unsigned int)stackp )
        stackp++;

    int space = (unsigned int)stackp - (unsigned int)stack;
    return space;
}

int thread_create(tcb *cb, char *stack, int stacksize, char priority, int flags, void (*function) (void), const char* name)
{
    /* stacksize must be a multitude of 4 for alignment and stacktest */
//  assert( ((stacksize & 0x03) == 0) && (stacksize > 0) );

    // TODO: shall we autoalign the stack?
    // stacksize += 4-(~(stacksize & 0x0003));

    if (priority >= SCHED_PRIO_LEVELS) {
        return -EINVAL;
    }

    if (flags & CREATE_STACKTEST) {
        /* assign each int of the stack the value of it's address */
        unsigned int *stackmax = (unsigned int*) ((char*)stack + stacksize);
        unsigned int* stackp = (unsigned int*)stack;
        while(stackp < stackmax) {
            *stackp = (unsigned int)stackp;
            stackp++;
        }
    } else {
        /* create stack guard */
        *stack = (unsigned int)stack;
    }

    if (! inISR()) {
        dINT();
    }

    int pid = 0;
    while (pid < MAXTHREADS) {
        if (fk_threads[pid] == NULL) {
            fk_threads[pid] = cb;
            cb->pid = pid;
            break;
        }
        pid++;
    }

    if (pid == MAXTHREADS) {
        DEBUG("thread_create(): too many threads!\n");

        if (! inISR()) {
            eINT();
        }
        return -EOVERFLOW;
    }

    cb->sp = fk_stack_init(function,stack+stacksize);
    cb->stack_start = stack;
    cb->stack_size = stacksize;

    cb->priority = priority;
    cb->status = 0;

    cb->name = name;

    cb->wait_data = NULL;

    cb->msg_queue.data = 0;
    cb->msg_queue.priority = 0;
    cb->msg_queue.next = NULL;

    cb->rq_entry.data = (unsigned int) cb;
    cb->rq_entry.next = NULL;
    cb->rq_entry.prev = NULL;

    num_tasks++;

    DEBUG("Created thread %s. PID: %u. Priority: %u.\n", name, cb->pid, priority);

    if (flags & CREATE_SLEEPING) {
        sched_set_status(cb, STATUS_SLEEPING);
    } else {
        sched_set_status(cb, STATUS_PENDING);
        if (!(flags & CREATE_WOUT_YIELD)) {
            if (! inISR()) {
                eINT();
                fk_yield();
            } else {
                fk_context_switch_request = 1;
            }
        }
    }

    if (!inISR() && fk_thread!=NULL) {
        eINT();
    }

    return pid;
}
