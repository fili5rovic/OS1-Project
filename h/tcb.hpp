//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"

// Thread Control Block
class TCB
{
public:
    ~TCB() { delete[] stack; }

    bool isFinished() const { return finished; }

    bool isBlocked() const { return blocked; }

    void setFinished(bool value) { finished = value; }

    uint64 getTimeSlice() const { return timeSlice; }

    uint64 getSP() {return running->context.sp;}

    using Body = void (*)(void*);

    static void startTCB(TCB* tcb);

    static TCB *createThread(Body body, void* arg, void* stack);

    static TCB *createThreadNoStart(Body body, void* arg, void* stack);

    static uint64 thread_exit() {
        if (running->finished == true) {
            return -1;
        }
        running->setFinished(true);
        dispatch();
        return 0;
    }

    void setBlocked(const bool blocked) {
        this->blocked = blocked;
    }

    static void yield();

    static TCB *running;



private:
    TCB(Body body, void* arg, void* stack)
    {
        this->stack = (uint64*)stack;
        this->body = body;
        // this->stack = body != nullptr ? new uint64[STACK_SIZE] : nullptr;
        this->context = {(uint64) &threadWrapper,
                     this->stack != nullptr ? (uint64) &this->stack[STACK_SIZE] : 0
                    };
        this->timeSlice = TIME_SLICE;
        this->finished = false;
        this->arg = arg;
        this->blocked = false;
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    bool finished;
    void* arg;

    bool blocked;
    friend class Riscv;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);
public:
    static void dispatch();
private:
    static uint64 timeSliceCounter;

    static uint64 constexpr STACK_SIZE = 1024;
    static uint64 constexpr TIME_SLICE = 2;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
