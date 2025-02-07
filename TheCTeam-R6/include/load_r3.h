/**
 * @file load_r3.h
 * @brief Header file for loading R3 Functions
 *
 * This file contains the two core processes within R3 where one loads the processes and the other 
 * initializes each new processes context.
 */
#include <pcb.h>

/**
 * @brief This function loads test processes from 'processes.h'.
 * Each process is loaded and queued in a non-suspended ready state.
 * 
 * @param suspended Flag that indicates if process should be suspend. Non-0 if true.
 */
void load_processes(int suspended, int priority);
/**
 * @brief 
 * 
 * @param process The pointer to the process that is being initialized.
 * @param function_ptr The pointer to a the address of the function the process runs. 
 * @param suspended Flag that indicates if process should be suspend. Non-0 if true.
 */
void initialize_context(pcb* process, void (*function_ptr)(void), int suspended);
