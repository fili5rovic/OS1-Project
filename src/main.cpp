//
// Created by fili5rovic on 9/13/24.
//


#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../h/tcb.hpp"
#include "../h/KSem.hpp"

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

TCB* kernel = nullptr;
TCB* user = nullptr;

void test(void*) {
    // print("Unesi: \n");
    char t = getc();
    getc(); // enter
    printDebug("t: ", t);
}

int main() {
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    // Riscv::ms_sstatus(Riscv::SSTATUS_SIE); // OTVARA tajmer

    kernel = TCB::createThread(nullptr, nullptr,mem_alloc(DEFAULT_STACK_SIZE));
    kernel->setPrivilege(TCB::SUPERVISOR);
    TCB::running = kernel;

    // sem_open(&sem,0);
    // TCB* consumerThread = TCB::createThread(consumer, nullptr, mem_alloc(DEFAULT_STACK_SIZE));
    // TCB* producerThread = TCB::createThread(producer, nullptr, mem_alloc(DEFAULT_STACK_SIZE));
    // TCB* producerThread1 = TCB::createThread(producer, nullptr, mem_alloc(DEFAULT_STACK_SIZE));
    //
    // while (!(consumerThread->isFinished() && producerThread->isFinished() && producerThread1->isFinished())) {
    //     thread_dispatch();
    // }
    //
    // sem_close(sem);
    // user = TCB::createThread(reinterpret_cast<void (*)(void *)>(producerConsumer_C_API), nullptr, mem_alloc(DEFAULT_STACK_SIZE));
    thread_create(&user, test, nullptr);
    user->setPrivilege(TCB::SUPERVISOR); // has to be on for 7
    while(!user->isFinished()) {
        thread_dispatch();
    }

    // user = TCB::createThread(test, nullptr, mem_alloc(DEFAULT_STACK_SIZE));
    // user->setPrivilege(TCB::SUPERVISOR); // has to be on for 7
    // while(!user->isFinished()) {
    //     thread_dispatch();
    // }


    // print("Main finished...\n");
    return 0;
}
