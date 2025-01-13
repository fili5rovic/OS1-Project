#ifndef KSEM_H
#define KSEM_H

#include "../lib/hw.h"
#include "../h/list.hpp"
#include "../h/tcb.hpp"

class KSem {
public:
    explicit KSem(const int v) : val(v), closed(false){}

    static KSem* create(int v = 1);

    int signal();

    int wait();

    int trywait();

    int close();

private:
    int val;
    bool closed;
    List<TCB> blocked;

    void block();

    void unblock();
};

#endif //KSEM_H
