//
// Created by fili5rovic on 9/13/24.
//


#include "../h/KMemoryAllocator.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
#include "../h/tcb.hpp"
#include "../h/workers.hpp"

#include "../h/Threads_C_API_test.hpp"

#include "../lib/mem.h"

// void threadTest() {
//     TCB* threads[5];
//
//     threads[0] = TCB::createThread(nullptr, nullptr);
//     TCB::running = threads[0];
//
//     threads[1] = TCB::createThread(workerBodyA, nullptr);
//     // thread_create(&threads[1], workerBodyA, nullptr);
//     print("ThreadA created\n");
//     threads[2] = TCB::createThread(workerBodyB, nullptr);
//     print("ThreadB created\n");
//     threads[3] = TCB::createThread(workerBodyC, nullptr);
//     print("ThreadC created\n");
//     threads[4] = TCB::createThread(workerBodyD, nullptr);
//     print("ThreadD created\n");
//
//     while (!(threads[1]->isFinished() &&
//              threads[2]->isFinished() &&
//              threads[3]->isFinished() &&
//              threads[4]->isFinished())) {
//         thread_dispatch();
//              }
//
//     for (auto &thread: threads) {
//         delete thread;
//     }
//     print("Finished\n");
// }

void test(void* arg) {
    print("test\n");
}

void userMain(void* arg) {
    // print("Hi\n");
    void* stack = __mem_alloc(DEFAULT_STACK_SIZE * sizeof(uint64*));
    if (!stack) {
        print("Failed to allocate stack\n");
    }
    TCB* testThread = TCB::createThread(test, nullptr, stack);
    while(!testThread->isFinished()) {
        thread_dispatch();
    }

    Threads_C_API_test();
}


int main() {
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE); // OTVARA tajmer


    TCB* mainThread = TCB::createThread(nullptr, nullptr,mem_alloc(DEFAULT_STACK_SIZE));
    TCB::running = mainThread;

    // Threads_C_API_test();

    TCB* userMainThread = TCB::createThread(userMain, nullptr,mem_alloc(DEFAULT_STACK_SIZE));
    while(!userMainThread->isFinished()) {
        thread_dispatch();
    }

    // threadTest();


    // uint64* addr = (uint64*) mem_alloc(sizeof(uint64));
    // *addr = 3;
    // printDebug("Value: ", *addr);
    //
    // mem_free(addr);

    print("Main finished...\n");
    return 0;
}
