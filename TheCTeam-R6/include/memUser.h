/**
 * @file memUser.h
 * @brief Header file for user-level memory management functions.
 *
 * This header file provides declarations for memory management functions
 * used to allocate, free, and display memory blocks in the heap. These
 * functions interact with the underlying memory manager by calling 
 * appropriate functions to allocate and free memory and to display 
 * allocated and free blocks.
 */

#ifndef MEMUSER_H
#define MEMUSER_H

#include <stddef.h>

/**
 * @brief Allocates a block of memory from the heap.
 *
 * This function calls `allocate_memory()` to allocate a block of memory 
 * of the specified size. If successful, it prints the address of the newly 
 * allocated block in hexadecimal format (not the address of the MCB).
 * If allocation fails, it prints an error message.
 *
 * @param size The size of the memory allocation request in bytes (decimal).
 * @return void
 */
void allocate_memory_user(size_t size);

/**
 * @brief Frees a previously allocated block of memory.
 *
 * This function calls `free_memory()` to release a specified memory block 
 * back to the heap. If the block is freed successfully, the function 
 * completes silently. If freeing fails, it prints an error message.
 *
 * @param address The address of the memory block to free (in hexadecimal), 
 *                which should point to the start address of the allocated block (not the MCB).
 * @return void
 */
void free_memory_user(int address);

/**
 * @brief Displays information about all allocated memory blocks.
 *
 * This function walks through the list of allocated memory blocks and 
 * prints information about each one. The information includes:
 * - The start address of the block (in hexadecimal).
 * - The size of the block (in decimal).
 *
 * @return void
 */
void show_allocated_memory(void);

/**
 * @brief Displays information about all free memory blocks.
 *
 * This function walks through the list of free memory blocks and 
 * prints information about each one. The information includes:
 * - The start address of the block (in hexadecimal).
 * - The size of the block (in decimal).
 *
 * @return void
 */
void show_free_memory(void);

#endif // MEMUSER_H
