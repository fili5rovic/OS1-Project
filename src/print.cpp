//
// Created by os on 10/15/24.
//

#include "../h/print.h"

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
}
