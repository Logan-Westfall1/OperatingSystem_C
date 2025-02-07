/**
 @file sys_call.h
 @brief Kernel function for generating process context and system calls
*/
#ifndef SYSCALL_H
#define SYSCALL_H
#include <pcb.h>


/**
 * @struct context
 * @brief Represents the context of a process, including its register values.
 */
typedef struct context {    /**< ESP register (stack pointer) */
    unsigned int gs;     /**< GS segment register */
    unsigned int fs;     /**< FS segment register */
    unsigned int es;     /**< ES segment register */
    unsigned int ds;     /**< DS segment register */
    unsigned int ss;     /**< SS segment register */
    unsigned int eax;             /**< EAX register */
    unsigned int ebx;             /**< EBX register */
    unsigned int ecx;             /**< ECX register */
    unsigned int edx;             /**< EDX register */
    unsigned int edi;             /**< EDI register */
    unsigned int esi;             /**< ESI register */
    unsigned int ebp;             /**< EBP register */
    unsigned int eip;    /**< EIP register (instruction pointer) */
    unsigned int cs;     /**< CS segment register */
    unsigned int eflags; /**< EFLAGS register */
    unsigned int esp;
} context;

/* Global variables */
extern struct pcb* current_process;         /**< Pointer to the currently running process */
extern struct context* initial_context; /**< Pointer to the initial context */

/**
 * @brief Handles system calls and manages process context switching.
 *
 * @param context Pointer to the current process context.
 * @return Pointer to the context to be loaded for the next process.
 */
struct context* sys_call(struct context* context);
#endif // SYSCALL_H
