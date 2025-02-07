
#include <stdlib.h>
#include <io_scheduler.h>



// Function to enqueue an iocb to the dcb queue
void enqueue_iocb(dcb* dcb, iocb* iocb) {
    if (dcb->iocb_queue_tail == NULL) {
        dcb->iocb_queue_head = iocb;
    }
    else {
        dcb->iocb_queue_tail->next = iocb;
    }
    dcb->iocb_queue_tail = iocb;
}

// Function to dequeue an iocb from the dcb queue
iocb* dequeue_iocb(dcb* dcb) {
    if (dcb->iocb_queue_head == NULL) {
        return NULL;
    }
    iocb* iocb = dcb->iocb_queue_head;
    dcb->iocb_queue_head = dcb->iocb_queue_head->next;
    if (dcb->iocb_queue_head == NULL) {
        dcb->iocb_queue_tail = NULL;
    }
    return iocb;
}

// Function to handle I/O requests
int io_scheduler(int operation, device dev, char* buffer, size_t size, pcb* pcb, int waiting) {
    // Validate parameters
    if (operation != READ && operation != WRITE) {
        return ERR_INVALID_OPERATION;
    }

    if (!isValidDevice(dev)) {
        return ERR_INVALID_DEVICE;
    }

    if (buffer == NULL) {
        return ERR_INVALID_BUFFER_ADDRESS;
    }

    if (size == 0) {
        return ERR_INVALID_COUNT;
    }

    dcb* dcb = dcb_array[get_dcb_index(dev) - 1];

    if (!dcb->open) {
        return ERR_PORT_NOT_OPEN;
    }

    // Check the status of the requested device
    if (dcb->status == DCB_IDLE && !waiting) {
        // Device is available, begin processing the request immediately
        dcb->status = (operation == READ) ? DCB_READING : DCB_WRITING;
        dcb->input_buffer = buffer;
        dcb->output_buffer = buffer;
        dcb->input_size = size;
        dcb->output_size = size;
        dcb->event_flag = EVENT_FLAG_CLEAR;

        if (operation == READ) {
            serial_input_interrupt(dcb);
        }
        else {
            serial_output_interrupt(dcb);
        }
    }
    else {
        // Device is busy, enqueue the request in the iocb queue
        iocb* iocb = sys_alloc_mem(sizeof(struct iocb));
        if (iocb == NULL) {
            return ERR_MEMORY_ALLOCATION;
        }

        iocb->operation = operation;
        iocb->buffer = buffer;
        iocb->length = size;
        iocb->next = NULL;
        iocb->process = pcb;
        iocb->waiting = waiting;

        enqueue_iocb(dcb, iocb);
    }

    return SUCCESS;
}

// Function to process the next iocb in the queue
void process_next_iocb(dcb* dcb) {
    iocb* next_iocb = dequeue_iocb(dcb);
    if (next_iocb != NULL && !next_iocb->waiting) {
        dcb->status = (next_iocb->operation == READ) ? DCB_READING : DCB_WRITING;
        dcb->input_buffer = next_iocb->buffer;
        dcb->output_buffer = next_iocb->buffer;
        dcb->input_size = next_iocb->length;
        dcb->output_size = next_iocb->length;
        dcb->event_flag = EVENT_FLAG_CLEAR;
        pcb_remove(next_iocb->process);
        next_iocb->process->exec_state = READY;
        pcb_insert(next_iocb->process);

        if (next_iocb->operation == READ) {
            iocb_free(next_iocb);
            serial_input_interrupt(dcb);
        }
        else {
            iocb_free(next_iocb);
            serial_output_interrupt(dcb);
        }

        
    }
    else if(next_iocb != NULL && next_iocb->waiting)
    {
        dcb->input_buffer = next_iocb->buffer;
        dcb->output_buffer = next_iocb->buffer;
        dcb->input_size = next_iocb->length;
        dcb->output_size = next_iocb->length;
        dcb->event_flag = EVENT_FLAG_CLEAR;
        pcb_remove(next_iocb->process);
        next_iocb->process->exec_state = READY;
        pcb_insert(next_iocb->process);
        iocb_free(next_iocb);
    }
    else
    {
        dcb->status = DCB_IDLE;
    }




}

int iocb_free(iocb* iocb)
{
    if (iocb == NULL)
    {
        return 1;
    }

    return sys_free_mem(iocb);
}

