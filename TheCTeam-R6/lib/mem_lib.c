#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>

#include <mpx/serial.h>
#include <processes.h>
#include <sys_req.h>
#include <comHandler.h>
#include <mpx/io.h>
#include <sys_call.h>
#include <mem_lib.h>
#include <mpx/vm.h>


mcb *heap_head = NULL;  //head of the list

void initialize_heap(size_t size) {
    //allocates memory for the initial block
    mcb *initial_block = (mcb *) sys_alloc_mem(size + sizeof(mcb));

    if (initial_block == NULL) {
        print_e("Error: Failed to allocate memory for initial block");
        return;
    }

    //sets up the initial free block MCB
    initial_block->start_addr = (void *)((char *)initial_block + sizeof(mcb)); // Start of usable memory
    initial_block->size = size;
    initial_block->status = FREE;
    initial_block->next = NULL;
    initial_block->prev = NULL;

    heap_head = initial_block;
}



//initializes memory and places it in the list. Possibly splits a free block in half.
void *allocate_memory(size_t size) {
    mcb *current = heap_head;

    //traverses the list to find a suitable free block
    while (current != NULL) {
        //checks if block is free and large enough
        if (current->status == FREE && current->size >= size) {

            //splits the block if there's enough space
            if (current->size > size + sizeof(mcb)) {

                //calculate address for the new MCB
                mcb *new_block = (mcb *) ((char *)current->start_addr + size);

                //sets up the new MCB
                new_block->start_addr = (void *)((char *)new_block + sizeof(mcb));
                new_block->size = current->size - size - sizeof(mcb);
                new_block->status = FREE;
                new_block->next = current->next;
                new_block->prev = current;

                //updates the next block's prev pointer if it exists
                if (current->next != NULL) {
                    current->next->prev = new_block;
                }

                //links the new block into the list
                current->next = new_block;

                //adjusts the size of the current block
                current->size = size;
            }
            current->status = ALLOCATED;
            return current->start_addr;
        }
        current = current->next;
    }

    // No suitable block found
    return NULL;
}



//frees memory and updates memory block to free in the list. Also merges into adjecent free blocks.
int free_memory(void* address) {
    struct mcb *current = heap_head;

    //finds the mcb that matches to the given address
    while (current != NULL) {
        if (current->start_addr == (int*)address) {
            //mark block as free
            current->status = FREE;



            //merges with the next block if it's free
            if (current->next != NULL) {
                mcb *next_block = current->next;

                if (next_block->status == FREE) {
                    //merges current block with the next free block
                    current->size += sizeof(mcb) + next_block->size;
                    current->next = next_block->next;


                    //links the node after the next_block to the current node
                    if (next_block->next != NULL) {
                        mcb *next_next_block = next_block->next;
                        next_next_block->prev = current;
                    }
                }
            }



            //merges with the previous block if it's free
            if (current->prev != NULL) {
                struct mcb *prev_block = current->prev;

                if (prev_block->status == FREE) {
                    //merges previous block with the current free block
                    prev_block->size += sizeof(mcb) + current->size;
                    prev_block->next = current->next;

                    //links current (now prev_block) with the next node
                    if (current->next != NULL) {
                        mcb *next_block = current->next;
                        next_block->prev = prev_block;
                    }
                    //moves current to the merged block
                    current = prev_block;
                }
            }
            return 0;
        }
        current = current->next;
    }
    return -1; 
}


