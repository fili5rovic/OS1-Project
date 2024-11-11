//
// Created by marko on 20.4.222..
//

#include "../h/scheduler.hpp"

List<CCB> Scheduler::readyThreadQueue;

CCB* Scheduler::get() {
    return readyThreadQueue.removeFirst();
}

void Scheduler::put(CCB* ccb) {
    readyThreadQueue.addLast(ccb);
}
