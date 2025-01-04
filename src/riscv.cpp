//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../lib/mem.h"

void Riscv::popSppSpie() {
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

uint64 Riscv::syscall(uint64* args) {
    uint64 ret = 0;
    uint64 code = args[0];

    switch(code) {
        case 1: {
            ret = (uint64)__mem_alloc(args[1]);
            // ret = (uint64) KMemoryAllocator::getInstance().allocate(args[1]);
            break;
        }
        case 2: {
            ret = KMemoryAllocator::getInstance().free((void*)args[1]);
            break;
        }
        case 0x11: {
            using Body = void (*)(void*);
            TCB** tcb = (TCB**) args[1];
            Body body = Body(args[2]);
            void* arg = (void*)args[3];
            void* stack = (void*)args[4];
            *tcb = TCB::createThread(body, arg,stack);
            ret = *tcb == nullptr? 3:0;
            break;
        }
        case 0x12:
            ret = TCB::thread_exit();
            break;
        case 0x13: {
            TCB::dispatch();
            break;
        }
        default:
            break;
    }

    return ret;
}

void Riscv::handleSupervisorTrap() {
    uint64 args[5];
    loadParams(args);

    uint64 a1 = r_a1();

    const uint64 scause = r_scause();

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
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();
        // TCB::dispatch(); //UNCOMMENT FOR THREAD TEST

        w_a0(syscall(args));

        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else
    {
        print("\n---- ERROR ----\n");
        printDebug("scause: ", scause);
        printDebug("sepc: ", r_sepc());
        printDebug("stval: ", r_stval());
        print("---------------\n");

        // STOP Emulator
        __asm__ volatile("li t0, 0x5555");
        __asm__ volatile("li t1, 0x100000");
        __asm__ volatile("sw t0, 0(t1)");
    }

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
