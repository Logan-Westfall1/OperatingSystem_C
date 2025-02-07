/**
 @file pcbUser.h
 @brief Kernel function for all user commands regarding PCBs
*/

#ifndef PCBUSER_H
#define PCBUSER_H

#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include <mpx/serial.h>
#include <processes.h>
#include <sys_req.h>
#include <comHandler.h>
#include <mpx/io.h>
#include <pcb.h>

//function prototypes
/**
 * @brief Displays all processes and their:
 * name, class, state, suspended status, and priority.
 * @return 1 when process is complete.
 */
int showAll(void);
/**
 * @brief Checks if the process name is unique and valid.
 * 
 * @param processName 
 * @return 1 if name is valid, 0 if invalid. 
 */
int isValidName(char* processName);
/**
 * @brief Checks if the process class is unique and valid.
 * 
 * @param processClass 
 * @return 1 if class is valid, 0 if invalid
 */
int isValidClass(int processClass);
/**
 * @brief Checks if the priority is available and valid.
 * 
 * @param processPriority 
 * @return 1 if the priority is avalible and valid, 0 if not.
 */
int isValidPriority(int processPriority);
/**
 * @brief Calls pcb_setup() to create a PCB and inserts it in the approriate queue
 * with pcb_insert()
 * @param processName  
 * @param processClass 
 * @param processPriority 
 * @return 1 when process is comlete. 0 if unable to complete the process.
 */
int createPCB(char* processName, int processClass, int processPriority);
/**
 * @brief Finds and removes the secified process from its queue with pcb_remove()
 * and frees all memory associated with the process using pcb_free().
 * @param processName 
 * @return 1 when process is complete. 0 if unable to complete the process.
 */
int deletePCB(char* processName);
/**
 * @brief Puts the specified process in the blocked state and moves it to the 
 * appropriate queue.
 * @param processName 
 * @return 1 when process is complete. 0 if unable to complete the process.
 */
int suspendPCB(char* processName);
/**
 * @brief Puts a process in the not suspended and moves it to the 
 * appropriate queue.
 * @param processName 
 * @return 1 when process is complete. 0 if unable to complete the process.
 */
int resumePCB(char* processName);
/**
 * @brief Changes a specified process;s priority and moves it to the
 * appropriate spot in the the appropriate queue.
 * @param processName 
 * @param newPriority 
 * @return 1 when process is complete. 0 if unable to complete the process. 
 */
int setPCBPriority(char* processName, int newPriority);
/**
 * @brief Displays the specified process's: name, class, state, suspended status,
 * and priority.
 * @param processName 
 * @return 1 when process is complete. 0 if unable to complete the process.
 */
int showPCB(char* processName);
/**
 * @brief Displays the processes in the specified state.
 * 
 * @param state 
 */
void displayProcessesInState(int state);
/**
 * @brief For each process in the ready state, displays the process's:
 * name, class, state, suspended status, and priority
 * @return 1 when process is complete.
 */
int showReady(void);
/**
 * @brief For each process in the blocked state, displays each process's: 
 * name, class, state, suspended status, priority
 * @return 1 when process is complete. 
 */
int showBlocked(void);
/**
 * @brief Prints the specified process's:
 * name, class, state, and status.
 * @param pcb 
 */
void print_pcb(pcb* pcb);


#endif /* PCBUSER_H */
