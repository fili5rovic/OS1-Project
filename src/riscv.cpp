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

void Riscv::handleTimerInterrupt() {
    uint64 volatile sepc = r_sepc();
    uint64 volatile sstatus = r_sstatus();
    mc_sip(SIP_SSIP);


    TCB::timeSliceCounter++;
    if (TCB::timeSliceCounter >= TCB::running->getTimeSlice()) {
        TCB::dispatch();
    }

    w_sstatus(sstatus);
    w_sepc(sepc);
}

extern int thread_exit();

void Riscv::handleSupervisorTrap() {
    uint64 savedSP = TCB::running->oldSP;
    TCB::running->oldSP = r_sscratch();

    uint64 scause = r_scause();
    if (scause == TIMER_INTERRUPT) handleTimerInterrupt();
    else if (scause == CONSOLE_INTERRUPT) console_handler();
        // else if(scause & (1ul << 63)) {
        //     stopEmulator();
        // }
    else handleInternalInterrupts(scause);

    TCB::running->oldSP = savedSP;
}

void Riscv::handleInternalInterrupts(uint64 scause) {
    if (scause == USER_INTERRUPT || scause == SYSTEM_INTERRUPT) {
        uint64 volatile sstatus = r_sstatus();
        uint64 volatile sepc = r_sepc() + 4;
        uint64 syscall = r_a_index_stack(0);
        uint64 ret = 0;
        switch (syscall) {
            case MEM_ALLOC: {
                ret = (uint64) __mem_alloc(r_a_index_stack(1));
                // ret = (uint64) KMemoryAllocator::getInstance().allocate(r_a_index_stack(1));
                break;
            }
            case MEM_FREE: {
                // ret = KMemoryAllocator::getInstance().free((void*) r_a_index_stack(1));
                ret = (uint64) __mem_free((void*) r_a_index_stack(1));
                break;
            }
            case THREAD_START: {
                TCB* tcb = (TCB*) r_a_index_stack(1);
                TCB::startTCB(tcb);
                ret = 0;
                break;
            }
            case THREAD_CREATE: {
                using Body = void (*)(void*);
                TCB** tcb = (TCB**) r_a_index_stack(1);
                Body body = Body(r_a_index_stack(2));
                void* arg = (void*) r_a_index_stack(3);
                void* stack = (void*) r_a_index_stack(4);
                *tcb = TCB::createThread(body, arg, stack);
                // (*tcb)->setPrivilege(TCB::SUPERVISOR); // todo ??? kad ovo otvorim, radi mi 3 i 4 test, ali 7 ne puca a treba
                ret = *tcb == nullptr ? 3 : 0;
                break;
            }
            case THREAD_CREATE_NO_START: {
                using Body = void (*)(void*);
                TCB** tcb = (TCB**) r_a_index_stack(1);
                Body body = Body(r_a_index_stack(2));
                void* arg = (void*) r_a_index_stack(3);
                void* stack = (void*) r_a_index_stack(4);
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
                int val = (int) r_a_index_stack(2);
                KSem** sem = (KSem**) r_a_index_stack(1);
                *sem = KSem::create(val);
                ret = *sem == nullptr ? 2 : 0;
                break;
            }
            case SEM_CLOSE: {
                KSem* sem = (KSem*) r_a_index_stack(1);
                ret = sem->close();
                break;
            }
            case SEM_WAIT: {
                KSem* sem = (KSem*) r_a_index_stack(1);
                ret = sem->wait();
                break;
            }
            case SEM_SIGNAL: {
                KSem* sem = (KSem*) r_a_index_stack(1);
                ret = sem->signal();
                break;
            }
            case SEM_TIMEDWAIT: {
                KSem* sem = (KSem*) r_a_index_stack(1);
                ret = sem->trywait();
                break;
            }
            case GETC: {
                char c = __getc();
                ret = (uint64) c;
                break;
            }
            case PUTC: {
                ret = 0;
                char c = (char) r_a_index_stack(1);
                __putc(c);
                break;
            }
            default:
                print("WRONG Code\n");
                break;
        }
        w_a0_stack(ret);
        w_sstatus(sstatus);
        w_sepc(sepc);
    } else {
        // thread_exit();
        print("\n---- ERROR ----\n");
        printDebug("scause: ", scause);
        printDebug("sepc: ", r_sepc());
        printDebug("stval: ", r_stval());
        print("---------------\n");

        // stop emulator
        __asm__ volatile("li t0, 0x5555");
        __asm__ volatile("li t1, 0x100000");
        __asm__ volatile("sw t0, 0(t1)");
    }
}

void Riscv::w_a0_stack(uint64 a0) {
    uint64 addr = TCB::running->oldSP + 80;
    __asm__ volatile("sd %[a0], 0(%[addr])" : : [a0] "r"(a0), [addr] "r"(addr));
}

uint64 Riscv::r_a_index_stack(int index) {
    uint64 addr = TCB::running->oldSP + 80 + 8 * index;
    uint64 ret;
    __asm__ volatile("ld %[ret], 0(%[addr])" : [ret]"=r"(ret) : [addr]"r"(addr));
    return ret;
}
