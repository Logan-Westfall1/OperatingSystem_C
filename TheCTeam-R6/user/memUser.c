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


void address_print(int num)
{
    print("0x");

    char hex_chars[] = "0123456789abcdef";
    char* output = (char*)sys_alloc_mem(9 * sizeof(char));  // 8 hex digits + null terminator
    if (output == NULL) {
        return;  // Handle memory allocation failure
    }

    int leading_zero = 1;  // Flag to skip leading zeros
    int index = 0;

    // Fill the output array with hexadecimal characters
    for (int i = 7; i >= 0; i--) {
        char hex_digit = hex_chars[(num >> (i * 4)) & 0xF];

        if (hex_digit != '0' || !leading_zero || i == 0) {  // Only print after first non-zero
            leading_zero = 0;
            output[index++] = hex_digit;
        }
    }
    output[index] = '\0';  // Null-terminate the string

    print(output);
    println("");
    free_memory(output);
}

void allocate_memory_user(int size)
{
    void *allocated_address = allocate_memory((size_t)size);


    if (allocated_address != NULL) {
        print(YELLOW("Memory Allocated at address: "));
        address_print((int)allocated_address);
    } else {
        print_e("Error: Memory allocation failed.\n");
    }
}

void free_memory_user(void* address)
{
    void *memory = (int *)free_memory(address);

    if (memory == 0) {

        print(YELLOW("Memory successfully freed at address: "));
        address_print((int)address);
    } else {
        print(RED("Error: Failed to free memory at address: "));
        address_print((int)address);
    }
}

void show_allocated_memory(void)
{
    mcb *current = heap_head;
    char size[20];
    println(CYAN("Allocated Memory Blocks:"));
    while (current != NULL) {
        if (current->status == ALLOCATED) {
            itoa(current->size, size);
            print(YELLOW("Address: "));
            address_print((int)current->start_addr);
            print(YELLOW("Size: "));
            print(size);
            println("");
        }
        current = current->next;
    }
}

void show_free_memory(void)
{
    mcb *current = heap_head;
    char size[20];
    println(CYAN("Free Memory Blocks:"));
    while (current != NULL) {
        if (current->status == FREE) {
            itoa(current->size, size);
            print(YELLOW("Address: "));
            address_print((int)current->start_addr);
            print(YELLOW("Size: "));
            print(size);
            println("");
        }
        current = current->next;
    }
}

