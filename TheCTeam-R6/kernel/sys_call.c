//new C file for interrupt function
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
#include <pcb.h>
#include <io_scheduler.h>





pcb* current_process = NULL;
context* original_context = NULL;

context* sys_call(context* new_context) {

    int EAX = new_context->eax;
    for (int i = 0; i < 4; i++) {
        dcb* dcb = dcb_array[i];
        if (dcb != NULL && dcb->event_flag == EVENT_FLAG_SET) {
            // Reset event flag
            dcb->event_flag = EVENT_FLAG_CLEAR;

            // Process the next IOCB in the queue, if any
            process_next_iocb(dcb);

        }

    }
    // If EAX is IDLE, the process gives up CPU control temporarily
    if (EAX == IDLE) {

        if (original_context == NULL) {
            original_context = new_context;
        }

        pcb* temp = ready_head;
        if (temp == NULL) {
            new_context->eax = 0;
            return new_context;
        }
        else {
            // If there is a running process, save its state and add it to ready queue
            if (current_process != NULL) {
                current_process->stack_pointer = (unsigned char*)new_context;
                current_process->exec_state = READY;
                temp = ready_head;
                pcb_insert(current_process);
            }
            current_process = temp;
            pcb_remove(current_process);
            current_process->exec_state = RUNNING;
            new_context->eax = 0;
            return (context*)current_process->stack_pointer;
        }
    }

    // If EAX is EXIT, terminate the process and load next process
    else if (EAX == EXIT) {
        pcb* temp = ready_head;
        if (temp == NULL) {
            new_context->eax = 0;
            context* temporary = original_context;
            current_process = NULL;
            original_context = NULL;
            return temporary;
        }
        else {
            pcb_remove(temp);
            pcb_free(current_process);
            current_process = temp;
            new_context->eax = 0;
            return current_process->stack_pointer;
        }
    }

    /*
    * Device is in EBX
    * Buffer in ECX
    * Buffer Size in EDX
    * Check to see if device is in use
    * If not, directly call serial_read() or serial_write()
    * Otherwise, use I/O scheduler:
    * When scheduling I/O ops, move process to BLOCKED state
    * Dispatch new process as though requested operation was IDLE
    * Each byte transferred will generate an interrupt
    *  Device driver ISR will be called
    *  If operation is complete, set Event Flag in approproaite DCB
    * Each time sys_call() is called, check for any available Event Flags
    *  For any that are set, perform the required completion sequence
    */
    else if (EAX == READ)
    {
        device dev = (device)new_context->ebx;
        char* buffer = (char*)new_context->ecx;
        size_t size = (size_t)new_context->edx;

        // Validate device and check status
        dcb* dcb = dcb_array[get_dcb_index(dev) - 1];
        /*if (dcb->status == DCB_READING && dcb->input_buffer - buffer <= 100)
        {
            return new_context;
        }*/
        if (dcb->status != DCB_IDLE) {
            // If the device is busy, add request to I/O scheduler and block the process
            if (current_process != NULL) {
            if (!strcmp(current_process->name, "sysidle"))
            {
                new_context->eax = serial_read(dev, buffer, size);
                return new_context;
            }
            
            
                io_scheduler(READ, dev, buffer, size, current_process, 0);
                current_process->stack_pointer = (unsigned char*)new_context;
                current_process->exec_state = BLOCKED;
                pcb_insert(current_process);
            }

            // Dispatch new process as though EAX == IDLE
            pcb* temp = ready_head; // Get the head of the ready queue
            if (temp == NULL) {
                new_context->eax = 0;  // No ready processes, stay idle
                return new_context;
            }
            else {
                // If a process exists in the ready queue, schedule it
                current_process = temp;       // Get the next process
                pcb_remove(current_process);  // Remove it from the ready queue
                current_process->exec_state = RUNNING;  // Mark it as running
                new_context->eax = 0;         // Clear EAX
                return (context*)current_process->stack_pointer; // Return its context
            }
        }
        else {
            // Device is available; start the read operation
            new_context->eax = serial_read(dev, buffer, size);
            if (dcb->event_flag == EVENT_FLAG_CLEAR)
            {
                io_scheduler(READ, dev, buffer, size, current_process, 1);
                current_process->stack_pointer = (unsigned char*)new_context;
                current_process->exec_state = BLOCKED;
             
                pcb_insert(current_process);
                pcb* temp = ready_head; // Get the head of the ready queue
                if (temp == NULL) {
                    new_context->eax = 0;  // No ready processes, stay idle
                    return new_context;
                }
                else {
                    // If a process exists in the ready queue, schedule it
                    current_process = temp;       // Get the next process
                    pcb_remove(current_process);  // Remove it from the ready queue
                    current_process->exec_state = RUNNING;  // Mark it as running
                    new_context->eax = 0;         // Clear EAX
                    return (context*)current_process->stack_pointer; // Return its context
                }
            }
            return new_context;
        }

        // Context switch to next available process
        /*pcb* temp = ready_head;
        if (temp == NULL) {
            new_context->eax = 0;
            return new_context;
        }
        else {
            if (current_process != NULL) {
                current_process->stack_pointer = (unsigned char*)new_context;
                current_process->exec_state = READY;
                pcb_insert(current_process);
            }
            current_process = temp;
            pcb_remove(current_process);
            current_process->exec_state = RUNNING;
            return (context*)current_process->stack_pointer;
        }*/
    }

    else if (EAX == WRITE)
    {
        device dev = (device)new_context->ebx; //Works
        char* buffer = (char*)new_context->ecx; //Works
        size_t size = (size_t)new_context->edx; //Works

        dcb* dcb = dcb_array[get_dcb_index(dev) - 1]; //Works
        if (dcb->status != DCB_IDLE)
        {
            if (current_process != NULL) {
            if (!strcmp(current_process->name, "sysidle"))
            {
                new_context->eax = serial_write(dev, buffer, size);
                return new_context;
            }
                io_scheduler(WRITE, dev, buffer, size, current_process, 0);
                current_process->stack_pointer = (unsigned char*)new_context;
                current_process->exec_state = BLOCKED;
                pcb_insert(current_process);
            }

            // Dispatch new process as though EAX == IDLE
            pcb* temp = ready_head; // Get the head of the ready queue
            if (temp == NULL) {
                new_context->eax = 0;  // No ready processes, stay idle
                return new_context;
            }
            else {
                // If a process exists in the ready queue, schedule it
                current_process = temp;       // Get the next process
                pcb_remove(current_process);  // Remove it from the ready queue
                current_process->exec_state = RUNNING;  // Mark it as running
                new_context->eax = 0;         // Clear EAX
                return (context*)current_process->stack_pointer; // Return its context
            }
        }
        else
        {
            new_context->eax = serial_write(dev, buffer, size);
            return new_context;
        }

    }

    // Check each DCB for completed I/O operations
    
    new_context->eax = -1;
    return new_context;
}
