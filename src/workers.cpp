//
// Created by os on 10/29/24.
//

#include "../lib/hw.h"
#include "../h/ccb.hpp"
#include "../h/print.hpp"

static uint64 fibonacci(uint64 n) {
    if(n == 0 || n == 1) return n;
    if(n % 4 == 0) CCB::yield();
    return fibonacci(n-1) + fibonacci(n-2);
}

void workerBodyA() {
    uint8 i = 0;
    for(; i < 3; i++) {
        print("A: i=");
        printInt(i);
        print("\n");
    }

    print("A: yield\n");
    __asm__("li t1, 7");
    CCB::yield();

    uint64 t1 = 0;
    __asm__ ("mv %[t1], t1" : [t1] "=r"(t1));

    print("A: t1=");
    printInt(t1);
    print("\n");

    uint64 result = fibonacci(20);
    print("A: fibonaci=");
    printInt(result);
    print("\n");

    for (; i < 6; i++)
    {
        print("A: i=");
        printInt(i);
        print("\n");
    }

    CCB::running->setFinished(true);
    CCB::yield();
}

void workerBodyB() {
    uint8 i = 10;
    for(; i < 13; i++) {
        print("B: i=");
        printInt(i);
        print("\n");
    }

    print("B: yield\n");
    __asm__("li t1, 5");
    CCB::yield();

    uint64 result = fibonacci(20);
    print("A: fibonaci=");
    printInt(result);
    print("\n");

    for (; i < 16; i++) {
        print("B: i=");
        printInt(i);
        print("\n");
    }

    CCB::running->setFinished(true);
    CCB::yield();
}
