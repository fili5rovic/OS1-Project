//
// Created by marko on 20.4.22..
//

#include "../lib/hw.h"
#include "../h/tcb.hpp"
#include "../h/print.hpp"

void workerBodyA(void* arg)
{
    for (uint64 i = 0; i < 10; i++)
    {
        print("A: i=");
        printInt(i);
        print("\n");
        for (uint64 j = 0; j < 10000; j++)
        {
            for (uint64 k = 0; k < 30000; k++)
            {
                // busy wait
            }
//            TCB::yield();
        }
    }
}

void workerBodyB(void* arg)
{
    for (uint64 i = 0; i < 16; i++)
    {
        print("B: i=");
        printInt(i);
        print("\n");
        for (uint64 j = 0; j < 10000; j++)
        {
            for (uint64 k = 0; k < 30000; k++)
            {
                // busy wait
            }
//            TCB::yield();
        }
    }
}

static uint64 fibonacci(uint64 n)
{
    if (n == 0 || n == 1) { return n; }
    if (n % 10 == 0) { TCB::yield(); }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

void workerBodyC(void* arg)
{
    uint8 i = 0;
    for (; i < 3; i++)
    {
        print("C: i=");
        printInt(i);
        print("\n");
    }

    print("C: yield\n");
    __asm__ ("li t1, 7");
    TCB::yield();

    uint64 t1 = 0;
    __asm__ ("mv %[t1], t1" : [t1] "=r"(t1));

    print("C: t1=");
    printInt(t1);
    print("\n");

    uint64 result = fibonacci(12);
    print("C: fibonaci=");
    printInt(result);
    print("\n");

    for (; i < 6; i++)
    {
        print("C: i=");
        printInt(i);
        print("\n");
    }
//    TCB::yield();
}

void workerBodyD(void* arg)
{
    uint8 i = 10;
    for (; i < 13; i++)
    {
        print("D: i=");
        printInt(i);
        print("\n");
    }

    print("D: yield\n");
    __asm__ ("li t1, 5");
    TCB::yield();

    uint64 result = fibonacci(16);
    print("D: fibonaci=");
    printInt(result);
    print("\n");

    for (; i < 16; i++)
    {
        print("D: i=");
        printInt(i);
        print("\n");
    }
//    TCB::yield();
}