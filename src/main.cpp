//
// Created by os on 9/13/24.
//

#include "../h/KMemoryAllocator.h"
#include "../lib/console.h"

int main() {
    void *a = KMemoryAllocator::getInstance().allocate(sizeof(char));
    char* b = static_cast<char *>(a);
    *b = 'a';
    __putc(*b);
    __putc('\n');
    __putc('\n');
    return 0;
}