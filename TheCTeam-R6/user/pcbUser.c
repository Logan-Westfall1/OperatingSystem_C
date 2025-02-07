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
#include "pcbUser.h"

//ProcessNamme validation values, randomly chosen and can be changed
#define MIN_NAME_LENGTH 1
#define MAX_NAME_LENGTH 10


//Validation functions. If these are short/simple enough we can just code them into the createPCB function

//Checks if name is unique and valid
int isValidName(char* processName)
{
    if (strlen(processName) >= MIN_NAME_LENGTH && strlen(processName) <= MAX_NAME_LENGTH)
    {
        return 1;
    }
    else
    {
        print_e("Error: Invalid process name");
        return 0;
    }
}

//Checks if class is valid
int isValidClass(int processClass)
{
    if (processClass == 0 || processClass == 1)
    {
        return 1;
    }
    else
    {
        print_e("Error: Invalid process class");
        return 0;
    }
}

//Checks if priority is valid
int isValidPriority(int processPriority)
{
    if (processPriority >= 0 && processPriority <= 9)
    {
        return 1;
    }
    else
    {
        print_e("Error: Invalid process priority");
        return 0;
    }
}




/*
* Finds requested process, removes it from its queue with pcb_remove()
*
* Name must be valid
* Must NOT be a system process
*/
int deletePCB(char* processName) {
    if (!isValidName(processName))
    {
        return 0;
    }

    pcb* pcb = pcb_find(processName);

    if(pcb == NULL)
    {
        print_e("Error: PCB Does not exist");
        return 0;
    }

    if(pcb->class == SYSTEM_PROCESS){
        print_e("Error: Cannot delete SYSTEM_PROCESS");
        return 0;
    }

    if (pcb == NULL)
    {
        return 0;
    }

    pcb_remove(pcb);
    pcb_free(pcb);
    println(GREEN("PCB successfully deleted"));

    return 1;
}


/*
* Puts a process in the suspended state, and moves it to the appropriate queue
*
* Name must be valid
* Must NOT be a system process
*/
int suspendPCB(char* processName) {
    if (!isValidName(processName))
    {
        return 0;
    }

    

    pcb* pcb = pcb_find(processName);

    if(pcb == NULL)
    {
        print_e("Error: PCB Does not exist");
        return 0;
    }


    if(pcb->class == SYSTEM_PROCESS){
        print_e("Error: Cannot suspend SYSTEM_PROCESS");
        return 0;
    }

    if (pcb == NULL)
    {
        print_e("Error: PCB does not exist");
        return 0;
    }

    if(pcb->disp_state == SUSPENDED){
        println(YELLOW("PCB is already in a SUSPENDED state"));
        return 0;
    }

    // Set process to suspended state
    

    pcb_remove(pcb);
    pcb->disp_state = SUSPENDED;
    pcb_insert(pcb);

    println(GREEN("PCB successfully suspended"));

    // Move to suspended queue?


    return 1;
}

/*
* Puts a process in the not suspended state, and moves it to the appropriate queue
*
* Name must be valid
*/
int resumePCB(char* processName) {
    if (!isValidName(processName))
    {
        return 0;
    }

    pcb* pcb = pcb_find(processName);

    if(pcb == NULL)
    {
        print_e("Error: PCB Does not exist");
        return 0;
    }

    if (pcb == NULL)
    {
        print_e("Error: PCB does not exist");
        return 0;
    }

    if(pcb->disp_state == NOT_SUSPENDED){
        println(YELLOW("PCB is already in a NOT_SUSPENDED state"));
        return 0;
    }

    // Set process to suspended state

    pcb_remove(pcb);
    pcb->disp_state = NOT_SUSPENDED;
    pcb_insert(pcb);

    println(GREEN("PCB successfully resumed"));
    //Move to ready queue?

    return 1;
}

/*
* Changes a process's priority, and moves it to the appropriate plae in the appropriate queue
*
* Name must be valid
* Priority must be valid (0-9)
*/
int setPCBPriority(char* processName, int newPriority) {
    if (!isValidName(processName))
    {
        return 0;
    }
    if (!isValidPriority(newPriority))
    {
        return 0;
    }

    pcb* pcb = pcb_find(processName);

    if(pcb == NULL)
    {
        print_e("Error: PCB Does not exist");
        return 0;
    }

    if(pcb->class == SYSTEM_PROCESS){
        print_e("Error: Cannot change priority of a SYSTEM_PROCESS");
        return 0;
    }
    if (pcb == NULL)
    {
        print_e("Error: PCB does not exist");
        return 0;
    }

    // Set new priority
    pcb_remove(pcb);
    pcb->priority = newPriority;
    pcb_insert(pcb);

    print(GREEN("PCB priority successfully changed to: "));
    char pri_buff[2];
    itoa(newPriority,pri_buff);
    println(pri_buff);
    


    return 1;
}

/*
* Displays a process's:
*   Name
*   Class
*   State
*   Suspended Status
*   Priority
*
* Name must be valid
*/
int showPCB(char* processName) {
    if (!isValidName(processName))
    {
        return 0;
    }

    pcb* pcb = pcb_find(processName);
    if (pcb == NULL)
    {
        print_e("Error: PCB does not exist");
        return 0;
    }
    print_pcb(pcb);


    return 1;
}


void displayProcessesInState(int state)
{
    pcb* current;

    switch(state)
    {
        case 0:
            current = ready_head;
            break;
        case 2:
            current = blocked_head;
            break;
        default:
            current = NULL;
            print_e("Error: Invalid State");
            return;
    }
        if (current == NULL)
        {
            //Print message saying no processes in chosen state
            print_e("Error: No Processes in chosen state");
            return;
        }

    while (current != NULL)
    {
        print_pcb(current);
        current = current->next_pcb;
    }
    return;
}

/*
* For all processes in the Ready state, display the process's:
*   Name
*   Class
*   State
*   Suspended Status
*   Priority
*
* No error checking
*/
int showReady(void) {
    char* running_ready = CYAN("Printing Ready Queue:\n");
    char* running_suspended_ready = CYAN("Printing Suspended Ready Queue:\n");


    pcb* current = ready_head;
    sys_req(WRITE, COM1, running_ready, strlen(running_ready));

    if(current == NULL){
        print_e("This queue is empty");
    }
    else
    {
        while (current != NULL)
        {
            print_pcb(current);
            current = current->next_pcb;
        }
    }


    current = ready_suspended_head;
    sys_req(WRITE, COM1, running_suspended_ready, strlen(running_suspended_ready));

    if(current == NULL){
        print_e("This queue is empty");
    }
    else
    {
        while (current != NULL)
        {
            print_pcb(current);
            current = current->next_pcb;
        }
    }
    return 1;
}

/*
* For all processes in the Blocked state, display the process's:
*   Name
*   Class
*   State
*   Suspended Status
*   Priority
*
* No error checking
*/
int showBlocked(void) {
    char* running_blocked = CYAN("Printing Blocked Queue:\n");
    char* running_suspended_blocked = CYAN("Printing Suspended Blocked Queue:\n");


    pcb* current = blocked_head;
    sys_req(WRITE, COM1, running_blocked, strlen(running_blocked));

    if(current == NULL){
        print_e("This queue is empty");
    }
    else
    {
        while (current != NULL)
        {
            print_pcb(current);
            current = current->next_pcb;
        }
    }


    current = blocked_suspended_head;
    sys_req(WRITE, COM1, running_suspended_blocked, strlen(running_suspended_blocked));

    if(current == NULL){
        print_e("This queue is empty");
    }
    else
    {
        while (current != NULL)
        {
            print_pcb(current);
            current = current->next_pcb;
        }
    }
    return 1;
}

/*
* For all processes in any state, display the process's:
*   Name
*   Class
*   State
*   Suspended Status
*   Priority
*
* No error checking
*/
int showAll(void) {
    char* running_ready = CYAN("Printing Ready Queue:\n");
    char* running_suspended_ready = CYAN("Printing Suspended Ready Queue:\n");

    char* running_blocked = CYAN("Printing Blocked Queue:\n");
    char* running_suspended_blocked = CYAN("Printing Suspended Blocked Queue:\n");


    pcb* current = ready_head;
    sys_req(WRITE, COM1, running_ready, strlen(running_ready));

    if(current == NULL){
        print_e("This queue is empty");
    }
    else
    {
        while (current != NULL)
        {
            print_pcb(current);
            current = current->next_pcb;
        }
    }


    current = ready_suspended_head;
    sys_req(WRITE, COM1, running_suspended_ready, strlen(running_suspended_ready));

    if(current == NULL){
        print_e("This queue is empty");
    }
    else
    {
        while (current != NULL)
        {
            print_pcb(current);
            current = current->next_pcb;
        }
    }


    current = blocked_head;
    sys_req(WRITE, COM1, running_blocked, strlen(running_blocked));

    if(current == NULL){
        print_e("This queue is empty");
    }
    else
    {
        while (current != NULL)
        {
            print_pcb(current);
            current = current->next_pcb;
        }
    }


    current = blocked_suspended_head;
    sys_req(WRITE, COM1, running_suspended_blocked, strlen(running_suspended_blocked));

    if(current == NULL){
        print_e("This queue is empty");
    }
    else
    {
        while (current != NULL)
        {
            print_pcb(current);
            current = current->next_pcb;
        }
    }

    return 1;
}

void print_pcb(pcb* pcb)
{
    char* pcb_name = (char*) pcb->name;
    char* pcb_class;
    char* pcb_state;
    char* pcb_status;

//    Needs to have a functioning itoa
    
    char pri_buff[2];
    char* pcb_priority = itoa(pcb->priority, pri_buff);

    switch(pcb->class)
    {
        case 0:
            pcb_class = "USER_PROCESS";
            break;
        case 1:
            pcb_class = "SYSTEM_PROCESS";
            break;
        default:
            print_e("Error: Invalid Class when trying to print");
        return;
    } 

    switch(pcb->exec_state)
    {
        case 0:
            pcb_state = GREEN("READY");
            break;
        case 2:
            pcb_state = RED("BLOCKED");
            break;
        default:
            print_e("Error: Invalid State when trying to print");
            return;
    }
  
    switch(pcb->disp_state)
    {
        case 0:
            pcb_status = RED("SUSPENDED");
            break;
        case 1:
            pcb_status = GREEN("NOT SUSPENDED");
            break;
        default:
            print_e("Error: Invalid status when trying to print");
            return;
    }
    print(YELLOW("Name: "));
    println(pcb_name);
    print(YELLOW("Class: "));
    println(pcb_class);
    print(YELLOW("State: "));
    println(pcb_state);
    print(YELLOW("Status: "));
    println(pcb_status);
    print(YELLOW("Priority: "));
    println(pcb_priority);
    println("");
}




