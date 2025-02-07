/**
 * @file io_scheduler.h
 * @brief Header file for the I/O Scheduler module.
 * 
 * Provides structures and function declarations for managing I/O operations
 * and scheduling using IOCBs (I/O Control Blocks).
 */

#ifndef IO_SCHEDULER_H
#define IO_SCHEDULER_H

#include <stdlib.h>
#include <mpx/interrupts.h>
#include <serial_interrupts.h>

/** @name Error Codes
 * @{
 */
#define ERR_INVALID_OPERATION -1          /**< Error: Invalid operation type. */
#define ERR_MEMORY_ALLOCATION -1          /**< Error: Memory allocation failure. */
/** @} */

/**
 * @brief Structure representing an I/O Control Block (IOCB).
 */
typedef struct iocb {
    int operation;            /**< Operation type (e.g., READ or WRITE). */
    char* buffer;             /**< Pointer to the buffer for the I/O operation. */
    size_t length;            /**< Length of the buffer. */
    struct iocb* next;        /**< Pointer to the next IOCB in the queue. */
    struct pcb* process;      /**< Pointer to the associated process (PCB). */
    int waiting;              /**< Flag: 1 if waiting for completion, 0 if pending. */
} iocb;

/**
 * @brief Enqueues an IOCB to the specified DCB's queue.
 * 
 * @param dcb Pointer to the DCB where the IOCB will be enqueued.
 * @param iocb Pointer to the IOCB to be enqueued.
 */
void enqueue_iocb(dcb* dcb, iocb* iocb);

/**
 * @brief Dequeues an IOCB from the specified DCB's queue.
 * 
 * @param dcb Pointer to the DCB from which the IOCB will be dequeued.
 * @return Pointer to the dequeued IOCB, or NULL if the queue is empty.
 */
iocb* dequeue_iocb(dcb* dcb);

/**
 * @brief Schedules an I/O operation.
 * 
 * @param operation The type of operation (READ or WRITE).
 * @param dev The target device for the I/O operation.
 * @param buffer Pointer to the buffer for the operation.
 * @param size Size of the buffer.
 * @param pcb Pointer to the PCB of the associated process.
 * @param waiting Flag indicating if the operation requires waiting for completion.
 * @return SUCCESS if the operation is successfully scheduled, or an error code.
 */
int io_scheduler(int operation, device dev, char* buffer, size_t size, pcb* pcb, int waiting);

/**
 * @brief Processes the next IOCB in the specified DCB's queue.
 * 
 * @param dcb Pointer to the DCB whose IOCB queue will be processed.
 */
void process_next_iocb(dcb* dcb);

/**
 * @brief Frees an IOCB structure.
 * 
 * @param iocb Pointer to the IOCB to be freed.
 * @return SUCCESS if the IOCB is successfully freed, or an error code.
 */
int iocb_free(iocb* iocb);

#endif // IO_SCHEDULER_H
