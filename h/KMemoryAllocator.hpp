//
// Created by fili5rovic on 9/13/24.
//

#ifndef KMEMORYALLOCATOR_H
#define KMEMORYALLOCATOR_H

#include "../lib/hw.h"
#include "../h/print.hpp"

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
    void* allocate(size_t _size);

    /**
     * Frees a previously allocated block of memory and returns it to the free list.
     * @param ptr Pointer to the memory block to free.
     * @return 0 on success, or 1 if the pointer is invalid.
     */
    int free(void* ptr);

    /**
     * Prints the current state of the memory allocator with an accompanying message.
     * Useful for debugging and tracking memory usage.
     * @param message Message to print before the memory state.
     */
    void printStateWithMessage(const char* message) const;

    /**
     * Prints the current state of the free list, showing the memory blocks
     * and their sizes. Useful for tracking memory fragmentation.
     */
    void printState() const;

   private:
    KMemoryAllocator();

    // Helper functions for alloc()
    /**
     * Aligns the requested size to the nearest multiple of the memory block size to combat fragmentation.
     * @param _size The original requested size.
     * @return The size aligned to the block size, including space for the Node header.
     */
    size_t alignToBlockSize(size_t _size);

    /**
     * Checks if the current free block can be split after allocation.
     * Ensures that the remaining space is large enough to form a new free block.
     * @param current The current free node.
     * @param totalSize The total size (including header) requested for allocation.
     * @return True if the block can be split, false otherwise.
     */
    bool canSplitBlockByNewSize(Node* current, size_t totalSize);

    /**
     * Creates a new node from the remaining space after allocation.
     * @param current The current node being allocated from.
     * @param sizeOffset The size offset of the new allocation.
     * @return A new node representing the remaining free space.
     */
    Node* makeNewNodeFromCurrent(Node* current, uint64 sizeOffset);


    // Helper functions for free()
    /**
     * Adds node to free nodes linked list
     * @param node Node parameter that should be added to free node list
     * @return Returns non-zero if the execution fails.
     */
    int addFreeNode(Node* node);

    /**
     * Links node2 before node1. Also changes freeHead if it was node1.
     * <br> Before: x->node1->y
     * <br> After: x->node2->node1->y
     * @param node1 Will be node2's next
     * @param node2 Will be node1's prev, changes 'prev' and 'next' pointers
     */
    void linkAfter(Node* node1, Node* node2);

    /**
    * Attempts to merge two adjacent free nodes into a single larger node.
    * If successful, the function recursively continues merging the resulting node with previous nodes.
    *
    * @param smallerNode The node with the lower memory address. If the merge succeeds, this node will absorb largerNode.
    * @param largerNode The node with the higher memory address, which will be merged into smallerNode and destroyed.
    * @return 0 if merge succeeds, non-zero if the nodes are not adjacent or if either node is null.
    */
    int tryToMergeNodes(Node* smallerNode, Node* largerNode);

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
