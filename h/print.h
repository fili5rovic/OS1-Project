//
// Created by os on 9/21/24.
//

#ifndef HELPER_H
#define HELPER_H

#include "../lib/console.h"
#include "../lib/hw.h"

void printInt(const uint64 n) {
    if (n / 10) {
        printInt(n / 10);
    }

    __putc((n % 10) + '0');
}

void printNewLine() {
    __putc('\n');
}

void print(const char* s) {
    for (; *s != '\0'; s++) {
        __putc(*s);
    }
    // __putc('\n');
}


#endif //HELPER_H
