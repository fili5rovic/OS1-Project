#ifndef CCB_H
#define CCB_H

#include "../lib/hw.h"
#include "scheduler.hpp"

class CCB {
public:
    ~CCB() { delete[] stack;}
    using Body = void(*)();

    bool isFinished() const { return finished; }
    void setFinished(const bool f) { finished = f; }

    static CCB* createCoroutine(Body body);

    static void yield();

    static CCB* running;

private:
    CCB(Body body) :
    body(body),
    stack(body != nullptr ? new uint64[STACK_SIZE] : nullptr),
    context({
        body != nullptr ? (uint64) body: 0,
        stack != nullptr ? (uint64)&stack[STACK_SIZE] : 0
        }),
    finished(false)
    {
        if(body != nullptr) {Scheduler::put(this);}
    }

    struct Context {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    uint64* stack;
    Context context;
    bool finished;

    static void contextSwitch(Context* oldContext, Context* runningContext);

    static void dispatch();

    static uint64 constexpr STACK_SIZE = 1024;
};

#endif //CCB_H
