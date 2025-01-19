//
// Created by fili5rovic on 9/13/24.
//


#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../h/tcb.hpp"

extern void userMain();

TCB* kernel = nullptr;
TCB* user = nullptr;

void test(void* a) {
    print("Unesi: \n");
    char t = getc();
    // getc(); // enter
    printDebug("t: ", t);
}

int main() {
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    // Riscv::ms_sstatus(Riscv::SSTATUS_SIE); // OTVARA tajmer

    kernel = TCB::createThread(nullptr, nullptr,mem_alloc(DEFAULT_STACK_SIZE));
    kernel->setPrivilege(TCB::SUPERVISOR);
    TCB::running = kernel;

    char t = getc();
    putc(t);
    // user = TCB::createThread(test, nullptr, mem_alloc(DEFAULT_STACK_SIZE));
    // // thread_create(&user, test, nullptr);
    // // user->setPrivilege(TCB::SUPERVISOR); // has to be on for 7
    // while(!user->isFinished()) {
    //     thread_dispatch();
    // }

    // print("Main finished...\n");
    return 0;
}
