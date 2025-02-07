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

#define MIN_NAME_LENGTH 1
#define MAX_NAME_LENGTH 10


// Queue heads
pcb* ready_head = NULL;
pcb* blocked_head = NULL;
pcb* ready_suspended_head = NULL;
pcb* blocked_suspended_head = NULL;

// Returns pcb** to the correct head node. Must be dereferenced once to get an actual pointer to the usable pcb at that head.
pcb** get_queue(pcb* pcb) {

    if (pcb->exec_state == READY || pcb->exec_state == RUNNING) {
        if (pcb->disp_state == NOT_SUSPENDED) {
            return &ready_head;
        }
        else if (pcb->disp_state == SUSPENDED) {
            return &ready_suspended_head;
        }
    }
    else if (pcb->exec_state == BLOCKED) {
        if (pcb->disp_state == NOT_SUSPENDED) {
            return &blocked_head;
        }
        else if (pcb->disp_state == SUSPENDED) {
            return &blocked_suspended_head;
        }
    }
    return NULL; //Must return something or GDB gets mad
}



pcb* pcb_allocate(void) {
    //Might have to set this to a variable check if it is NULL then return it if we want an error message, otherwise check in a higher level function

    return sys_alloc_mem(sizeof(pcb));    // As stated in the memory.h file this function returns NULL on error, 
}                                         // otherwise returns the allocated memory address


int pcb_free(pcb* pcb) {
    //Same as allocate might need error message check in this function. Decide later.
    if (pcb == NULL) {
        return 1; // Avoid freeing a NULL pointer
    }

    // Free the allocated memory for the PCB's context
    if (pcb->context != NULL) {
        sys_free_mem(pcb->context);
    }

    // Free the allocated memory for the PCB's name
    if (pcb->name != NULL) {
        sys_free_mem((void*)pcb->name);
    }

    // Free the PCB structure itself
    return sys_free_mem(pcb);
}

pcb* pcb_setup(const char* name, int class, int priority) {
    if(pcb_find(name) != NULL)
    {
        print_e("Error: A PCB with this name already exists");
        return NULL;
    }

    pcb* pcb = pcb_allocate();

    if(pcb == NULL)
    {
        print_e("Error: Failed to allocate memory");
        return NULL;
    }

    
    pcb->context = sys_alloc_mem(sizeof(struct context));
    if (pcb->context == NULL)
    {
        print_e("Error: Failed to allocate memory for context");
        return NULL;
    }
    


    pcb->name = sys_alloc_mem(strlen(name) + 1); // +1 for the null terminator
    if (pcb->name == NULL) {
        print_e("Error: Failed to allocate memory for PCB name");
        pcb_free(pcb);
        return NULL;
    }
    pcb_find(name);


    strncpy((char*)pcb->name, name, strlen(name));

    pcb -> exec_state = READY;
    pcb -> disp_state = NOT_SUSPENDED;
    pcb -> next_pcb = NULL;

    if (class >= 0 && class <= 1) {
        pcb->class = class;
    }
    else {
        print_e("Error: Invalid class inputed");
        return NULL; // Need to do proper error handling
    }

    if (priority >= 0 && priority <= 9) {
        pcb->priority = priority;
    }
    else {
        print_e("Error: Invalid priority inputed");
        return NULL; // Need to do proper error handling
    }


    // Needs error handling or at least return NULL for error in allocating, initializing, or invalid parameter.

    if(strcmp(pcb->name, "comhand") == 0 || strcmp(pcb->name, "sysidle") == 0)
    {
        return pcb;
    }
    else
    {
        print(YELLOW("Creating Process: "));
        println((char*)name);
        return pcb;
    }
}

pcb* pcb_find(const char* name) {

    //check the ready queue
    pcb* current = ready_head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current;
        }
        current = current->next_pcb;
    }

    //check the ready suspended queue
    current = ready_suspended_head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current;
        }
        current = current->next_pcb;
    }

    //check the blocked queue
    current = blocked_head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current;
        }
        current = current->next_pcb;
    }

    //check the blokcked suspended queue
    current = blocked_suspended_head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current;
        }
        current = current->next_pcb;
    }

    //couldnt find pcb
    return NULL;
}

void pcb_insert(pcb* new_pcb) {
    pcb** queue_head_ptr = get_queue(new_pcb);

    //checks if queue is valid
    if (queue_head_ptr == NULL) {
        print_e("Error: Invalid Queue");
        return;
    }

    //READY queue logic which is inserted based on priority
    if (new_pcb->exec_state == READY && new_pcb->disp_state == NOT_SUSPENDED) {
        pcb* current = ready_head;
        pcb* previous = NULL;

        //traverse queue to find the correct position
        while (current != NULL && current->priority <= new_pcb->priority) {
            previous = current;
            current = current->next_pcb;
        }

        //points the new pcb to the appropriate next pcb
        new_pcb->next_pcb = current;

        //checks to see if we are at the head
        if (previous == NULL) {
            ready_head = new_pcb;
        }
        else {
            previous->next_pcb = new_pcb;
        }

        //BLOCKED queue logic
    }
    else if(new_pcb->exec_state == READY && new_pcb->disp_state == SUSPENDED)
    {
        pcb* current = ready_suspended_head;
        pcb* previous = NULL;

        //traverse queue to find the correct position
        while (current != NULL && current->priority <= new_pcb->priority) {
            previous = current;
            current = current->next_pcb;
        }

        //points the new pcb to the appropriate next pcb
        new_pcb->next_pcb = current;

        //checks to see if we are at the head
        if (previous == NULL) {
            ready_suspended_head = new_pcb;
        }
        else {
            previous->next_pcb = new_pcb;
        }
    }
    else if (new_pcb->exec_state == BLOCKED && new_pcb->disp_state == NOT_SUSPENDED)
    {
        pcb* current = blocked_head;
        pcb* previous = NULL;

        //traverse queue to find the correct position
        while (current != NULL && current->priority <= new_pcb->priority) {
            previous = current;
            current = current->next_pcb;
        }

        //points the new pcb to the appropriate next pcb
        new_pcb->next_pcb = current;

        //checks to see if we are at the head
        if (previous == NULL) {
            blocked_head = new_pcb;
        }
        else {
            previous->next_pcb = new_pcb;
        }
    }
    else if (new_pcb->exec_state == BLOCKED && new_pcb->disp_state == SUSPENDED)
    {
        pcb* current = blocked_suspended_head;
        pcb* previous = NULL;

        //traverse queue to find the correct position
        while (current != NULL && current->priority <= new_pcb->priority) {
            previous = current;
            current = current->next_pcb;
        }

        //points the new pcb to the appropriate next pcb
        new_pcb->next_pcb = current;

        //checks to see if we are at the head
        if (previous == NULL) {
            blocked_suspended_head = new_pcb;
        }
        else {
            previous->next_pcb = new_pcb;
        }
    }
    else {

        if (*queue_head_ptr == NULL) {
            //if the queue is empty inserts at head
            *queue_head_ptr = new_pcb;
        }
        else {
            //traverses to the end of the queue
            pcb* current = *queue_head_ptr;
            while (current->next_pcb != NULL) {
                current = current->next_pcb;
            }
            //inserts at the end
            current->next_pcb = new_pcb;
        }
    }
}

int pcb_remove(pcb* pcb) {
    struct pcb** head = get_queue(pcb);
    if (head == NULL || *head == NULL) {
        print_e("Error: Queue is empty or PCB not in any known queue");
        return -1;
    }
    struct pcb* curPtr = *head;
    struct pcb* prevPtr = NULL;

    while (curPtr != NULL && curPtr != pcb) {
        prevPtr = curPtr;
        curPtr = curPtr->next_pcb;
    }

    if (curPtr == NULL) {
        print_e("Error: PCB not found");
        return -1;
    }

    if (prevPtr == NULL) {
        *head = curPtr->next_pcb;
    }
    else {
        prevPtr->next_pcb = curPtr->next_pcb;
    }

    curPtr->next_pcb = NULL;
    return 0;

}

void clear_queues(void) {
    pcb* current;
    pcb* temp;

    // Clear the ready queue
    current = ready_head;
    while (current != NULL) {
        temp = current;
        current = current->next_pcb;
        pcb_free(temp);  // Free the memory of the PCB
    }
    ready_head = NULL; // Set the head to NULL after clearing

    // Clear the blocked queue
    current = blocked_head;
    while (current != NULL) {
        temp = current;
        current = current->next_pcb;
        pcb_free(temp);
    }
    blocked_head = NULL;

    // Clear the ready suspended queue
    current = ready_suspended_head;
    while (current != NULL) {
        temp = current;
        current = current->next_pcb;
        pcb_free(temp);
    }
    ready_suspended_head = NULL;

    // Clear the blocked suspended queue
    current = blocked_suspended_head;
    while (current != NULL) {
        temp = current;
        current = current->next_pcb;
        pcb_free(temp);
    }
    blocked_suspended_head = NULL;
}


