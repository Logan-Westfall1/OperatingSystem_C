/**
 * @file mem_lib.h
 * @brief Memory management system using a single linked list of Memory Control Blocks (MCBs).
 *
 * This file contains function declarations and type definitions for a simple 
 * memory manager that uses Memory Control Blocks (MCBs) to track allocated 
 * and free memory within a single list structure.
 *
 * The memory manager provides functions to initialize the heap, allocate 
 * memory, and free memory, with adjacent free blocks merged automatically 
 * to reduce fragmentation.
 *
 * @details Functions:
 * - initialize_heap(): Initializes a single large free block for memory management.
 * - allocate_memory(): Allocates a block of memory, potentially splitting a free block.
 * - free_memory(): Frees a previously allocated block, merging with adjacent free blocks if possible.
 */

#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>

/**
 * @enum mcb_status
 * @brief Status of a memory block.
 *
 * This enum represents whether a memory block is free or allocated.
 */
typedef enum {
    FREE,         /**< Indicates the block is free and available for allocation. */
    ALLOCATED     /**< Indicates the block is currently allocated and in use. */
} mcb_status;

/**
 * @struct mcb
 * @brief Memory Control Block (MCB) structure.
 *
 * Each MCB represents a block of memory in the heap.
 * It contains information about the block's start address, size, status,
 * and pointers to the next and previous blocks in the linked list.
 */
typedef struct mcb {
    void *start_addr;     /**< Pointer to the start of the usable memory in the block. */
    size_t size;          /**< Size of the memory block in bytes (excluding MCB metadata). */
    mcb_status status;    /**< Status of the block (FREE or ALLOCATED). */
    struct mcb *next;     /**< Pointer to the next MCB in the list. */
    struct mcb *prev;     /**< Pointer to the previous MCB in the list. */
} mcb;

/**
 * @brief Head of the MCB linked list.
 *
 * This pointer represents the start of the memory block list used by the
 * memory manager. Initially set to NULL.
 */
extern mcb *heap_head;

/**
 * @brief Initializes the memory manager with a single large free block.
 *
 * This function creates an initial free block of hardcoded size (25,000 bytes)
 * and sets up the MCB for this block. The block is added to the head of the
 * memory manager's list.
 *
 * @return void
 */
void initialize_heap(size_t);

/**
 * @brief Allocates a block of memory of a specified size.
 *
 * This function searches the MCB list for a free block that can satisfy the 
 * requested size. If a suitable block is found, it marks it as allocated. 
 * If the block is larger than requested, it splits the block, creating a new 
 * free block with the remaining space.
 *
 * @param size The size of memory to allocate (in bytes).
 * @return void* Pointer to the start address of the allocated memory (NULL on failure).
 */
void *allocate_memory(size_t size);

/**
 * @brief Frees a previously allocated block of memory.
 *
 * This function marks a specified memory block as free, based on the given 
 * start address. If the freed block has adjacent free blocks, they are merged 
 * into a single larger free block to reduce fragmentation.
 *
 * @param address Pointer to the start address of the memory block to free.
 * @return int Returns 0 on successful freeing, or -1 if the block was not found.
 */
int free_memory(void* address);

#endif // MEMORY_MANAGER_H
