//
// Created by os on 9/21/24.
//
#ifndef HELPER_H
#define HELPER_H

#include "../lib/console.h"
#include "../lib/hw.h"
#include "../h/riscv.hpp"

// Inline implementation of printInt
inline void printInt(uint64 n) {
    uint64 sstatus = Riscv::r_sstatus();
    Riscv::mc_sstatus(Riscv::SSTATUS_SIE);

    char buffer[20];
    int index = 0;

    do {
        buffer[index++] = (n % 10) + '0';
        n /= 10;
    } while (n > 0);

    while (--index >= 0) {
        __putc(buffer[index]);
    }

    Riscv::ms_sstatus(sstatus & Riscv::SSTATUS_SIE ? Riscv::SSTATUS_SIE : 0);
}

// Inline implementation of printNewLine
inline void printNewLine() {
    __putc('\n');
}

// Inline implementation of print
inline void print(const char* s) {
    uint64 sstatus = Riscv::r_sstatus();
    Riscv::mc_sstatus(Riscv::SSTATUS_SIE);

    for (; *s != '\0'; s++) {
        __putc(*s);
    }

    Riscv::ms_sstatus(sstatus & Riscv::SSTATUS_SIE ? Riscv::SSTATUS_SIE : 0);
}

// Inline implementation of printDebug
inline void printDebug(const char* s, uint64 val) {
    print(s);
    printInt(val);
    printNewLine();
}

#endif // HELPER_H
