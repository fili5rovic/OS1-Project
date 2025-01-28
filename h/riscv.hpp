//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_RISCV_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_RISCV_HPP

#include "../lib/hw.h"


class Riscv {
public:

    // pop sstatus.spp and sstatus.spie bits (has to be a non inline function)
    static void popSppSpie();
    static void systemPopSppSpie();

    // read register scause
    static uint64 r_scause();

    enum SCAUSE {
        USER_INTERRUPT = 0x0000000000000008UL,
        SYSTEM_INTERRUPT = 0x0000000000000009UL,
        TIMER_INTERRUPT = 0x8000000000000001UL,
        CONSOLE_INTERRUPT = 0x8000000000000009UL
    };

    static uint64 r_a0();

    static uint64 r_a1();

    static void w_a0(uint64 a0);

    static void w_a1(uint64 a1);

    // write register scause
    static void w_scause(uint64 scause);

    // read register sepc
    static uint64 r_sepc();

    // write register sepc
    static void w_sepc(uint64 sepc);

    // read register stvec
    static uint64 r_stvec();

    // write register stvec
    static void w_stvec(uint64 stvec);

    // read register stval
    static uint64 r_stval();

    // write register stval
    static void w_stval(uint64 stval);

    enum BitMaskSip
    {
        SIP_SSIP = (1 << 1),
        SIP_STIP = (1 << 5),
        SIP_SEIP = (1 << 9),
    };

    // mask set register sip
    static void ms_sip(uint64 mask);

    // mask clear register sip
    static void mc_sip(uint64 mask);

    // read register sip
    static uint64 r_sip();

    // write register sip
    static void w_sip(uint64 sip);

    enum BitMaskSstatus
    {
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8),
    };

    enum SystemCode {
        MEM_ALLOC = 0x01,
        MEM_FREE = 0x02,

        THREAD_START = 0x09,
        THREAD_CREATE_NO_START = 0x10,
        THREAD_CREATE = 0x11,
        THREAD_EXIT = 0x12,
        THREAD_DISPATCH = 0x13,

        SEM_OPEN = 0x21,
        SEM_CLOSE = 0x22,
        SEM_WAIT = 0x23,
        SEM_SIGNAL = 0x24,
        SEM_TIMEDWAIT = 0x25,

        GETC = 0x41,
        PUTC = 0x42
    };


    // mask set register sstatus
    static void ms_sstatus(uint64 mask);

    // mask clear register sstatus
    static void mc_sstatus(uint64 mask);

    // read register sstatus
    static uint64 r_sstatus();

    // write register sstatus
    static void w_sstatus(uint64 sstatus);

    static void w_a0_stack(uint64);
    static uint64 r_a_index_stack(int index);
    static uint64 r_sscratch();

    // supervisor trap
    static void supervisorTrap();

private:

    static void handleSupervisorTrap();

    static void handleInternalInterrupts(uint64 scause);

    static void handleTimerInterrupt();

    static void loadParams(uint64*);

};

inline uint64 Riscv::r_scause()
{
    uint64 scause;
    __asm__ volatile ("csrr %[scause], scause" : [scause] "=r"(scause));
    return scause;
}

inline void Riscv::w_scause(uint64 scause)
{
    __asm__ volatile ("csrw scause, %[scause]" : : [scause] "r"(scause));
}

inline uint64 Riscv::r_sepc()
{
    uint64 volatile sepc;
    __asm__ volatile ("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
    return sepc;
}

inline void Riscv::w_sepc(uint64 sepc)
{
    __asm__ volatile ("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
}

inline uint64 Riscv::r_stvec()
{
    uint64 volatile stvec;
    __asm__ volatile ("csrr %[stvec], stvec" : [stvec] "=r"(stvec));
    return stvec;
}

inline void Riscv::w_stvec(uint64 stvec)
{
    __asm__ volatile ("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
}

inline uint64 Riscv::r_stval()
{
    uint64 volatile stval;
    __asm__ volatile ("csrr %[stval], stval" : [stval] "=r"(stval));
    return stval;
}

inline void Riscv::w_stval(uint64 stval)
{
    __asm__ volatile ("csrw stval, %[stval]" : : [stval] "r"(stval));
}

inline void Riscv::ms_sip(uint64 mask)
{
    __asm__ volatile ("csrs sip, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::mc_sip(uint64 mask)
{
    __asm__ volatile ("csrc sip, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::r_sip()
{
    uint64 volatile sip;
    __asm__ volatile ("csrr %[sip], sip" : [sip] "=r"(sip));
    return sip;
}

inline void Riscv::w_sip(uint64 sip)
{
    __asm__ volatile ("csrw sip, %[sip]" : : [sip] "r"(sip));
}

inline void Riscv::ms_sstatus(uint64 mask)
{
    __asm__ volatile("csrs sstatus, %0" : : "r"(mask));
}

inline void Riscv::mc_sstatus(uint64 mask)
{
    __asm__ volatile("csrc sstatus, %0" : : "r"(mask));
}

inline uint64 Riscv::r_sstatus()
{
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void Riscv::w_sstatus(uint64 sstatus)
{
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

inline uint64 Riscv::r_a0() {
    uint64 volatile a_ret;

    __asm__ volatile ("mv %0, a0" : "=r"(a_ret));
    return a_ret;
}

inline uint64 Riscv::r_a1() {
    uint64 volatile a_ret;

    __asm__ volatile (
        "mv %0, a1"
        : "=r"(a_ret)
        :
        : "a0", "a1", "a2", "a3", "a4", "a5"
    );
    return a_ret;
}

inline void Riscv::w_a0(uint64 a0) {
    __asm__ volatile ("mv a0, %0" : : "r"(a0));
}

inline void Riscv::w_a1(uint64 a1) {
    __asm__ volatile ("mv a1, %0" : : "r"(a1));
}

inline uint64 Riscv::r_sscratch() {
    uint64 volatile sscratch;
    __asm__ volatile("csrr %0, sscratch" : "=r"(sscratch));
    return sscratch;
}



inline void Riscv::loadParams(uint64 *arr)
{
    __asm__ volatile ("mv %0, a0" : "=r" (arr[0]));
    __asm__ volatile ("mv %0, a1" : "=r" (arr[1]));
    __asm__ volatile ("mv %0, a2" : "=r" (arr[2]));
    __asm__ volatile ("mv %0, a3" : "=r" (arr[3]));
    __asm__ volatile ("mv %0, a4" : "=r" (arr[4]));
}







#endif
