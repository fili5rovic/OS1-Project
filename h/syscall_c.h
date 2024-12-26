#ifndef C_SYSCALL_H
#define C_SYSCALL_H

#include "tcb.hpp"
#include "../lib/hw.h"
#include "../lib/console.h"

void* mem_alloc(size_t size);

int mem_free(void* ptr);

/**
 * Threads
 */
typedef TCB* thread_t;

int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg);

int thread_exit();

void thread_dispatch();

/**
 * Semaphore
 */



/**
 * Console
 */

inline char getc() {
    return __getc();
}

inline void putc(char c) {
    __putc(c);
}

#endif
