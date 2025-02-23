//
// Created by fili5rovic on 9/13/24.
//


#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../h/tcb.hpp"
#include "../lib/mem.h"

extern void userMain();

TCB* kernel = nullptr;
TCB* user = nullptr;

void test(void* a) {
    putc('f');
    putc('\n');
    print("Unesi: \n");
    char t = getc();
    getc(); // enter
    putc(t);
    putc('\n');
}

int main() {
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    // Riscv::ms_sstatus(Riscv::SSTATUS_SIE); // OTVARA tajmer

    kernel = TCB::createThread(nullptr, nullptr,__mem_alloc(DEFAULT_STACK_SIZE));
    kernel->setPrivilege(TCB::SUPERVISOR);
    TCB::running = kernel;
                            // reinterpret_cast<void (*)(void *)>(userMain)
    // char c = getc();
    // putc(c);
    // putc('\n');
    // test(nullptr);
    user = TCB::createThread(reinterpret_cast<void (*)(void *)>(userMain), nullptr, mem_alloc(DEFAULT_STACK_SIZE));
    // // thread_create(&user, test, nullptr);
    // user->setPrivilege(TCB::SUPERVISOR); // has to be on for 7 // kada ovo uradim ispisuje 5 a kada izbrisem ispisuje A
    while(!user->isFinished()) {
        thread_dispatch();
    }

    // print("Main finished...\n");
    return 0;
}
