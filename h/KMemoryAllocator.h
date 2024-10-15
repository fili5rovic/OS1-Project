//
// Created by fili5rovic on 9/13/24.
//

#ifndef KMEMORYALLOCATOR_H
#define KMEMORYALLOCATOR_H

#include "../lib/hw.h"
#include "../h/print.h"

/**
 * Struct representing a node in the memory management system.
 * Nodes track the size and pointers to the previous and next blocks of free memory.
 */
struct Node {
    Node* next;
    Node* prev;
    uint64 size;
};

/**
 * Singleton class for managing memory allocation and dealloca
 * tion.
 * Provides mechanisms for allocating memory, freeing memory blocks, and handling
 * a linked list of free memory blocks.
 */
class KMemoryAllocator {
public:
    /**
     * Returns the singleton instance of the KMemoryAllocator class.
     * @return Reference to the singleton instance.
     */
    static KMemoryAllocator &getInstance() {
        static KMemoryAllocator instance;
        return instance;
    }

    KMemoryAllocator(KMemoryAllocator const &) = delete;
    KMemoryAllocator(KMemoryAllocator &&) = delete;
    KMemoryAllocator &operator=(KMemoryAllocator const &) = delete;
    KMemoryAllocator &operator=(KMemoryAllocator &&) = delete;

    /**
    * Allocates a block of memory of the specified size using first-fit algorithm.
    * Attempts to find a free block large enough and, if successful, allocates memory from that block.
    * @param _size The size of memory to allocate.
    * @return Pointer to the allocated memory, or nullptr if no suitable block is available.
    */
    void* allocate(const size_t _size) {
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

    /**
     * Frees a previously allocated block of memory and returns it to the free list.
     * @param ptr Pointer to the memory block to free.
     * @return 0 on success, or 1 if the pointer is invalid.
     */
    int free(void* ptr) {
        if (ptr == nullptr)
            return 0;
        uint64 addr = reinterpret_cast<uint64>(ptr) - sizeof(Node);
        Node* node = reinterpret_cast<Node*>(addr);
        if (node == nullptr)
            return 1;
        return addFreeNode(node);
    }

    /**
     * Prints the current state of the memory allocator with an accompanying message.
     * Useful for debugging and tracking memory usage.
     * @param message Message to print before the memory state.
     */
    void printStateWithMessage(const char* message) {
        print(message);
        printState();
    }

    /**
     * Prints the current state of the free list, showing the memory blocks
     * and their sizes. Useful for tracking memory fragmentation.
     */
    void printState() const {
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

private:
    KMemoryAllocator() {
        nextAddress = reinterpret_cast<uint64>(HEAP_START_ADDR);
        freeHead = (Node*) ((char*) nextAddress);
        nextAddress += sizeof(Node);

        freeHead->next = nullptr;
        freeHead->prev = nullptr;
        freeHead->size = reinterpret_cast<uint64>(HEAP_END_ADDR) - nextAddress + sizeof(Node); // -sizeof(Node)
    }

    // Helper functions for alloc()
    /**
     * Aligns the requested size to the nearest multiple of the memory block size to combat fragmentation.
     * @param _size The original requested size.
     * @return The size aligned to the block size, including space for the Node header.
     */
    size_t alignToBlockSize(size_t _size) {
        size_t newSize = _size + sizeof(Node);
        if (newSize % MEM_BLOCK_SIZE != 0 && newSize <= 134182016) {
            newSize -= newSize % MEM_BLOCK_SIZE;
            newSize += MEM_BLOCK_SIZE;
        }
        return newSize;
    }
    /**
     * Checks if the current free block can be split after allocation.
     * Ensures that the remaining space is large enough to form a new free block.
     * @param current The current free node.
     * @param totalSize The total size (including header) requested for allocation.
     * @return True if the block can be split, false otherwise.
     */
    bool canSplitBlockByNewSize(Node* current, const size_t totalSize) {
        // ensure the remaining space is large enough to form a new free block
        return (current->size - totalSize >= MEM_BLOCK_SIZE &&
                (reinterpret_cast<uint64>(current) + totalSize + sizeof(Node)) <
                reinterpret_cast<uint64>(HEAP_END_ADDR));
    }

    /**
     * Creates a new node from the remaining space after allocation.
     * @param current The current node being allocated from.
     * @param sizeOffset The size offset of the new allocation.
     * @return A new node representing the remaining free space.
     */
    Node* makeNewNodeFromCurrent(Node* current, const uint64 sizeOffset) {
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


    // Helper functions for free()
    /**
     * Adds node to free nodes linked list
     * @param node Node parameter that should be added to free node list
     * @return Returns non-zero if the execution fails.
     */
    int addFreeNode(Node* node) {
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

    /**
     * Links node2 before node1. Also changes freeHead if it was node1.
     * <br> Before: x->node1->y
     * <br> After: x->node2->node1->y
     * @param node1 Will be node2's next
     * @param node2 Will be node1's prev, changes 'prev' and 'next' pointers
     */
    void linkAfter(Node* node1, Node* node2) {
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

    /**
    * Attempts to merge two adjacent free nodes into a single larger node.
    * If successful, the function recursively continues merging the resulting node with previous nodes.
    *
    * @param smallerNode The node with the lower memory address. If the merge succeeds, this node will absorb largerNode.
    * @param largerNode The node with the higher memory address, which will be merged into smallerNode and destroyed.
    * @return 0 if merge succeeds, non-zero if the nodes are not adjacent or if either node is null.
    */
    int tryToMergeNodes(Node* smallerNode, Node* largerNode) {
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

    // ----

    /**
     * freeHead for free nodes linked list
     */
    Node* freeHead;
    /**
     * This is the next address that gets returned in "alloc()"
     */
    uint64 nextAddress;
};


#endif //KMEMORYALLOCATOR_H
