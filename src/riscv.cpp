//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"

void Riscv::popSppSpie()
{
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSupervisorTrap()
{
    uint64 a1 = r_a1();

    uint64 scause = r_scause();

    w_a1(a1);

    if (scause == TIMER_INTERRUPT)
    {
        handleTimerInterrupt();
    }
    else if (scause == CONSOLE_INTERRUPT)
    {
        handleConsoleInterrupt();
    }
    else if (scause == USER_INTERRUPT || scause == SYSTEM_INTERRUPT)
    {
        handleInterrupts();

    }
    else
    {
        printDebug("SCAUSE:", scause);
        // unexpected trap cause
    }
}
void Riscv::handleInterrupts() {
    // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
    uint64 volatile sepc = r_sepc() + 4;
    uint64 volatile sstatus = r_sstatus();
    // TCB::dispatch(); //UNCOMMENT FOR THREAD TEST

    uint64 a0 = r_a0();
    uint64 a1 = r_a1();

    // printDebug("Interrupt: ", a0);

    switch(a0) {
        case 1: {
            uint64 size = a1;
            void* addr;
            addr = KMemoryAllocator::getInstance().allocate(size);

            // uint64 address = (uint64) addr;
            // printDebug("Address: ", address);

            w_a0((uint64)addr);

            break;
        }
        case 0x13: {
            break;
        }
        default:
            break;
    }

    w_sstatus(sstatus);
    w_sepc(sepc);
}

void Riscv::handleConsoleInterrupt() {
    // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
    console_handler();
}

void Riscv::handleTimerInterrupt() {
    // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
    mc_sip(SIP_SSIP);
    TCB::timeSliceCounter++;
    if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
    {
        uint64 volatile sepc = r_sepc();
        uint64 volatile sstatus = r_sstatus();

        TCB::dispatch();
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
}
