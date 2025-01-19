//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../lib/mem.h"
#include "../h/KSem.hpp"

void Riscv::popSppSpie() {
    Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::systemPopSppSpie() {
    Riscv::ms_sstatus(Riscv::SSTATUS_SPP);
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

uint64 Riscv::syscall(uint64* args) {
    uint64 ret = 0;
    uint64 code = args[0];

    switch (code) {
        case MEM_ALLOC: {
            ret = (uint64) __mem_alloc(args[1]);
            // ret = (uint64) KMemoryAllocator::getInstance().allocate(args[1]);
            break;
        }
        case MEM_FREE: {
            // ret = KMemoryAllocator::getInstance().free((void*) args[1]);
            ret = (uint64) __mem_free(args);
            break;
        }
        case THREAD_START: {
            TCB* tcb = (TCB*) args[1];
            TCB::startTCB(tcb);
            ret = 0;
            break;
        }
        case THREAD_CREATE: {
            using Body = void (*)(void*);
            TCB** tcb = (TCB**) args[1];
            Body body = Body(args[2]);
            void* arg = (void*) args[3];
            void* stack = (void*) args[4];
            *tcb = TCB::createThread(body, arg, stack);
            // (*tcb)->setPrivilege(TCB::SUPERVISOR); // todo ??? kad ovo otvorim, radi mi 3 i 4 test, ali 7 ne puca a treba
            ret = *tcb == nullptr ? 3 : 0;
            break;
        } case THREAD_CREATE_NO_START: {
            using Body = void (*)(void*);
            TCB** tcb = (TCB**) args[1];
            Body body = Body(args[2]);
            void* arg = (void*) args[3];
            void* stack = (void*) args[4];
            *tcb = TCB::createThreadNoStart(body, arg, stack);
            ret = *tcb == nullptr ? 3 : 0;
            break;
        }
        case THREAD_EXIT:
            ret = TCB::thread_exit();
            break;
        case THREAD_DISPATCH: {
            TCB::dispatch();
            break;
        }
        case SEM_OPEN: {
            int val = (int)args[2];
            KSem** sem = (KSem**) args[1];
            *sem = KSem::create(val);
            ret = *sem == nullptr ? 2 : 0;
            break;
        }
        case SEM_CLOSE: {
            KSem* sem = (KSem*) args[1];
            ret = sem->close();
            break;
        }
        case SEM_WAIT: {
            KSem* sem = (KSem*) args[1];
            ret = sem->wait();
            break;
        }
        case SEM_SIGNAL: {
            KSem* sem = (KSem*) args[1];
            ret = sem->signal();
            break;
        }
        case SEM_TIMEDWAIT: {
            KSem* sem = (KSem*) args[1];
            ret = sem->trywait();
            break;
        }
        case GETC: {
            // char c = '3';
            ret = '3';
            break;
        }
        case PUTC: {
            ret = 0;
            char c = (char) args[1];
            __putc(c);
            break;
        }
        default:
            print("WRONG Code\n");
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

    if (scause == TIMER_INTERRUPT) {
        handleTimerInterrupt();
    } else if (scause == CONSOLE_INTERRUPT) {
        handleConsoleInterrupt();
    } else if (scause == USER_INTERRUPT || scause == SYSTEM_INTERRUPT) {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();
        // TCB::dispatch(); //UNCOMMENT FOR THREAD TEST

        w_a0(syscall(args));

        w_sstatus(sstatus);
        w_sepc(sepc);
    } else {
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
    if (TCB::timeSliceCounter >= TCB::running->getTimeSlice()) {
        uint64 volatile sepc = r_sepc();
        uint64 volatile sstatus = r_sstatus();

        TCB::dispatch();
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
}
