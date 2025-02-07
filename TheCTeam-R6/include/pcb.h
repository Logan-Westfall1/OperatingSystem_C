/**
 * @file pcb.h
 * @brief Header file for Process Control Block (PCB) management in the operating system.
 *
 * This file contains the declarations of the PCB structure, enumeration types,
 * and function prototypes used for managing PCBs within the operating system.
 * The PCB represents a process in the system and holds all the necessary information
 * about the process's state, priority, class, and other attributes required for process management.
 */

#ifndef PCB_H
#define PCB_H

#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include <mpx/serial.h>
#include <processes.h>
#include <sys_req.h>
#include <comHandler.h>
#include <mpx/io.h>
#include <sys_call.h>

/**
 * @enum process_class
 * @brief Defines the class of a process.
 *
 * This enumeration specifies whether a process is a user process or a system process.
 */
typedef enum {
    USER_PROCESS,      /**< User process (0) */
    SYSTEM_PROCESS     /**< System process (1) */
} process_class;

/**
 * @enum execution_state
 * @brief Defines the execution state of a process.
 *
 * This enumeration specifies the current execution state of a process,
 * such as ready, running, or blocked.
 */
typedef enum {
    READY,             /**< Process is ready to run (0) */
    RUNNING,           /**< Process is currently running (1) */
    BLOCKED            /**< Process is blocked and cannot run (2) */
} execution_state;

/**
 * @enum dispatch_state
 * @brief Defines the dispatch state of a process.
 *
 * This enumeration specifies whether a process is suspended or not suspended.
 */
typedef enum {
    SUSPENDED,         /**< Process is suspended (0) */
    NOT_SUSPENDED      /**< Process is not suspended (1) */
} dispatch_state;

/**
 * @struct pcb
 * @brief Represents a Process Control Block (PCB).
 *
 * The PCB contains all the necessary information to manage a process within the operating system,
 * including its name, class, priority, execution state, dispatch state, stack, and pointers for queue management.
 */
typedef struct pcb {
    struct context* context;
    const char* name;                /**< Unique name of the process */
    process_class class;             /**< Class of the process (USER_PROCESS or SYSTEM_PROCESS) */
    int priority;                    /**< Priority of the process (0-9) */
    execution_state exec_state;      /**< Execution state (READY, RUNNING, or BLOCKED) */
    dispatch_state disp_state;       /**< Dispatch state (SUSPENDED or NOT_SUSPENDED) */
    unsigned char stack[1024];       /**< Stack memory allocated for the process (1024 bytes) */
    void* stack_pointer;             /**< Pointer to the current position in the stack */
    struct pcb* next_pcb;            /**< Pointer to the next PCB in the queue */
} pcb;

/**
 * @var ready_head
 * @brief Head of the ready queue for processes that are ready to run.
 */
extern pcb* ready_head;

/**
 * @var blocked_head
 * @brief Head of the blocked queue for processes that are blocked.
 */
extern pcb* blocked_head;

/**
 * @var ready_suspended_head
 * @brief Head of the suspended ready queue for processes that are ready but suspended.
 */
extern pcb* ready_suspended_head;

/**
 * @var blocked_suspended_head
 * @brief Head of the suspended blocked queue for processes that are blocked and suspended.
 */
extern pcb* blocked_suspended_head;

/**
 * @brief Retrieves the appropriate queue head pointer for the given process based on its state.
 *
 * This function determines which queue a process belongs to based on its execution and dispatch states,
 * and returns a pointer to the head of that queue.
 *
 * @param process Pointer to the PCB of the process.
 * @return Pointer to the head of the queue that the process belongs to, or NULL if invalid.
 */
pcb** get_queue(pcb* process);

/**
 * @brief Allocates memory for a new PCB.
 *
 * This function allocates memory for a new PCB structure using the system's memory allocation function.
 *
 * @return Pointer to the newly allocated PCB, or NULL if allocation fails.
 */
pcb* pcb_allocate(void);

/**
 * @brief Frees the memory allocated for a PCB.
 *
 * This function frees the memory occupied by a PCB structure, including any dynamically allocated fields within it.
 *
 * @param process Pointer to the PCB to be freed.
 * @return 0 on success, non-zero on failure.
 */
int pcb_free(pcb* process);

/**
 * @brief Creates and initializes a new PCB with the given parameters.
 *
 * This function sets up a new PCB with the specified name, class, and priority.
 * It allocates memory for the PCB, initializes its fields, and ensures that the process name is unique.
 *
 * @param name The unique name of the process.
 * @param class The class of the process (USER_PROCESS or SYSTEM_PROCESS).
 * @param priority The priority of the process (0-9).
 * @return Pointer to the newly created PCB, or NULL if creation fails.
 */
pcb* pcb_setup(const char* name, int class, int priority);

/**
 * @brief Searches for a PCB with the given name in all queues.
 *
 * This function searches through all the process queues to find a PCB with the specified name.
 *
 * @param name The name of the process to search for.
 * @return Pointer to the PCB if found, or NULL if not found.
 */
pcb* pcb_find(const char* name);

/**
 * @brief Inserts a PCB into the appropriate queue based on its state and priority.
 *
 * This function places the given PCB into the correct queue (ready, blocked, suspended, etc.)
 * according to its execution and dispatch states. For ready queues, it inserts the PCB based on priority.
 *
 * @param new_pcb Pointer to the PCB to be inserted.
 */
void pcb_insert(pcb* new_pcb);

/**
 * @brief Removes a PCB from its current queue.
 *
 * This function removes the specified PCB from the queue it is currently in.
 *
 * @param process Pointer to the PCB to be removed.
 * @return 0 on success, non-zero on failure.
 */
int pcb_remove(pcb* process);

void clear_queues(void);

#endif /* PCB_H */
