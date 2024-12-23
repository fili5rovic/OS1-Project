#ifndef C_SYSCALL_H
#define C_SYSCALL_H

#include "../lib/hw.h"
#include "../lib/console.h"

void* mem_alloc(size_t size);

int mem_free(void* ptr);

/**
 * Threads
 */


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
