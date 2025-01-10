//
// Created by fili5rovic on 9/13/24.
//


#include "../h/KMemoryAllocator.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
#include "../h/tcb.hpp"
#include "../h/KSem.hpp"

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

extern void userMain();

KSem* sem;

void consumer(void* arg) {
    print("Started consumer\n");
    sem_wait(sem);
    print("Finished consumer\n");

}
void producer(void* arg) {
    print("Started producer\n");
    sem_signal(sem);
    print("Finished producer\n");
}

TCB* kernel;
TCB* user;

int main() {
    // Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    // Riscv::ms_sstatus(Riscv::SSTATUS_SIE); // OTVARA tajmer

    // sem_open(&sem,1);
    //
    kernel = TCB::createThread(nullptr, nullptr,mem_alloc(DEFAULT_STACK_SIZE));
    kernel->setPrivilege(TCB::SUPERVISOR);
    TCB::running = kernel;

    // TCB* consumerThread = TCB::createThread(consumer, nullptr, mem_alloc(DEFAULT_STACK_SIZE));
    // TCB* producerThread = TCB::createThread(producer, nullptr, mem_alloc(DEFAULT_STACK_SIZE));
    //
    // while (!(consumerThread->isFinished() && producerThread->isFinished())) {
    //     thread_dispatch();
    // }
    //
    // sem_close(sem);
    user = TCB::createThread(reinterpret_cast<void (*)(void *)>(userMain), nullptr, mem_alloc(DEFAULT_STACK_SIZE));
    while(!user->isFinished()) {
        thread_dispatch();
    }


    // print("Main finished...\n");
    return 0;
}
