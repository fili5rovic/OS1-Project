//
// Created by os on 1/4/25.
//

#ifndef KSEM_H
#define KSEM_H

#include "../lib/hw.h"

class KSem {
public:
    explicit KSem(const uint64 v = 1) : val(v), closed(false) {}

    void operator++(int) {
        if (val == 0) {
            unblock();
        }
        val++;
    }

    void operator--(int) {
        val--;
        if (val == 0) {
            block();
        }
    }

    void close() {
        closed = true;
        // unblock every thread
    }

private:
    uint64 val;
    bool closed;

    void block() {

    }

    void unblock() {

    }

};

#endif //KSEM_H
