//
// Created by os on 10/15/24.
//

#include "../h/KMemoryAllocator.h"


void* KMemoryAllocator::allocate(const size_t _size) {
    const size_t newSize = alignToBlockSize(_size);
    // Helper::print("TRYING TO ALLOCATE: ");
    // Helper::printInt(newSize);
    // Helper::print("\n");

    Node* current = freeHead;
    while (current != nullptr) {
        // first fit
        if (current->size >= newSize) {
            // if current has more space, make another free node from the rest
            Node* nNode = nullptr;
            if (canSplitBlockByNewSize(current, newSize)) {
                nNode = makeNewNodeFromCurrent(current, newSize);
            }

            if (current->prev != nullptr)
                current->prev->next = nNode ? nNode : current->next;
            if (current->next != nullptr)
                current->next->prev = nNode ? nNode : current->prev;

            current->next = nullptr;
            current->prev = nullptr;
            current->size = newSize;

            if(current == freeHead && nNode == nullptr)
                freeHead = nullptr;

            nextAddress = reinterpret_cast<uint64>(current) + sizeof(Node);
            return reinterpret_cast<void*>(nextAddress);
        }
        current = current->next;
    }
    // no free block found
    nextAddress = 0;
    return nullptr;
}

int KMemoryAllocator::free(void* ptr) {
    if (ptr == nullptr)
        return 0;
    uint64 addr = reinterpret_cast<uint64>(ptr) - sizeof(Node);
    Node* node = reinterpret_cast<Node*>(addr);
    if (node == nullptr)
        return 1;
    return addFreeNode(node);
}

void KMemoryAllocator::printStateWithMessage(const char* message) const {
    print(message);
    printState();
}

void KMemoryAllocator::printState() const {
    Node* curr = freeHead;
    while (curr) {
        print("Node (");
        printInt(reinterpret_cast<uint64>(curr));
        print(")\tSize: ");
        printInt(curr->size);
        printNewLine();
        curr = curr->next;
    }
}

KMemoryAllocator::KMemoryAllocator() {
    nextAddress = reinterpret_cast<uint64>(HEAP_START_ADDR);
    freeHead = (Node*) ((char*) nextAddress);
    nextAddress += sizeof(Node);

    freeHead->next = nullptr;
    freeHead->prev = nullptr;
    freeHead->size = reinterpret_cast<uint64>(HEAP_END_ADDR) - nextAddress + sizeof(Node); // -sizeof(Node)
}

size_t KMemoryAllocator::alignToBlockSize(size_t _size) {
    size_t newSize = _size + sizeof(Node);
    if (newSize % MEM_BLOCK_SIZE != 0 && newSize <= 134182016) {
        newSize -= newSize % MEM_BLOCK_SIZE;
        newSize += MEM_BLOCK_SIZE;
    }
    return newSize;
}

bool KMemoryAllocator::canSplitBlockByNewSize(Node* current, const size_t totalSize) {
    // ensure the remaining space is large enough to form a new free block
    return (current->size - totalSize >= MEM_BLOCK_SIZE &&
            (reinterpret_cast<uint64>(current) + totalSize + sizeof(Node)) <
            reinterpret_cast<uint64>(HEAP_END_ADDR));
}

Node* KMemoryAllocator::makeNewNodeFromCurrent(Node* current, const uint64 sizeOffset) {
    if (current == nullptr)
        return nullptr;
    Node* node = reinterpret_cast<Node*>(reinterpret_cast<char*>(current) + sizeOffset);
    node->prev = current->prev;
    node->next = current->next;
    node->size = current->size - sizeOffset;
    if (current == freeHead)
        freeHead = node;
    return node;
}

int KMemoryAllocator::addFreeNode(Node* node) {
    if (freeHead == nullptr) {
        freeHead = node;
        return 0;
    }
    Node* current = freeHead;

    while (current != nullptr) {
        if (current == node) return 2;
        // add node before current if the current is bigger
        if (current > node) {
            linkAfter(current, node);
            tryToMergeNodes(node, current);
            return 0;
        }
        if (current->next == nullptr) break;
        current = current->next;
    }
    // add node to the end of the list
    current->next = node;
    node->prev = current;
    // check if you can merge them
    // tryToMergeNodes(current, node);
    return 0;
}

void KMemoryAllocator::linkAfter(Node* node1, Node* node2) {
    if (node1->prev != nullptr)
        node1->prev->next = node2;
    // if (node1->next != nullptr)
    //     node1->next->prev = node2;
    node2->next = node1;
    node2->prev = node1->prev;
    node1->prev = node2;
    if (node1 == freeHead)
        freeHead = node2;
}

int KMemoryAllocator::tryToMergeNodes(Node* smallerNode, Node* largerNode) {
    if (smallerNode == nullptr || largerNode == nullptr)
        return 1;
    uint64 diff = reinterpret_cast<uint64>(largerNode) - reinterpret_cast<uint64>(smallerNode);
    if (diff == smallerNode->size) {
        smallerNode->size += largerNode->size; // why not + sizeof(Node)?
        smallerNode->next = largerNode->next;
        largerNode->next = nullptr;
        largerNode->prev = nullptr;
        largerNode->size = 0;
        return tryToMergeNodes(smallerNode->prev, smallerNode);
    }
    return 2;
}
