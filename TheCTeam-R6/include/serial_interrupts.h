/**
 * @file serial_driver.h
 * @brief Header file for the Serial Driver module.
 * 
 * This file provides definitions, macros, and function declarations for
 * managing serial communication in the system.
 */

#ifndef SERIAL_DRIVER_H
#define SERIAL_DRIVER_H

#include <stddef.h>
#include <mpx/io.h>
#include <mpx/interrupts.h>
#include <comHandler.h>
#include <mpx/serial.h>
#include <sys_req.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <memory.h>
#include <sys_call.h>
#include <mpx/gdt.h>

/** @name Error Codes
 * @{
 */
#define SUCCESS 0                          /**< Operation completed successfully. */
#define ERR_INVALID_DEVICE -1              /**< Invalid device identifier. */
#define ERR_DEVICE_BUSY -304               /**< Device is currently busy. */
#define ERR_PORT_NOT_OPEN -201             /**< Port is not open. */
#define ERR_INVALID_EVENT -101             /**< Invalid event. */
#define ERR_INVALID_BAUD_DIVISOR -102      /**< Invalid baud rate divisor. */
#define ERR_PORT_ALREADY_OPEN -103         /**< Port is already open. */
#define ERR_INVALID_BUFFER_ADDRESS -302    /**< Invalid buffer address. */
#define ERR_INVALID_COUNT -303             /**< Invalid count value. */
/** @} */

/** @name Event Flag Definitions
 * @{
 */
#define EVENT_FLAG_SET 1                   /**< Event flag is set. */
#define EVENT_FLAG_CLEAR 0                 /**< Event flag is cleared. */
/** @} */

/** @name DCB Status Definitions
 * @{
 */
#define DCB_IDLE 0                         /**< DCB is idle. */
#define DCB_READING 1                      /**< DCB is reading data. */
#define DCB_WRITING 2                      /**< DCB is writing data. */
/** @} */

/** @name PIC Definitions
 * @{
 */
#define PIC_MASK_PORT 0x21                 /**< PIC mask port address. */
#define PIC_COMMAND_PORT 0x20              /**< PIC command port address. */
#define PIC_EOI 0x20                       /**< End of Interrupt signal. */
/** @} */





/** 
 * @brief Structure representing a Device Control Block (DCB).
 */
typedef struct dcb {
    int open;                   /**< Whether the device is open (1 for open, 0 for closed). */
    device device_id;           /**< Identifier for the device. */
    int event_flag;             /**< Event flag for signaling I/O completion. */
    int status;                 /**< Status: idle, reading, or writing. */
    char* input_buffer;         /**< Pointer to the current input buffer. */
    char* output_buffer;        /**< Pointer to the current output buffer. */
    size_t input_size;          /**< Size of the input buffer. */
    size_t output_size;         /**< Size of the output buffer. */
    char ring_buffer[256];      /**< Ring buffer for input storage. */
    size_t ring_head;           /**< Head index for the ring buffer. */
    size_t ring_tail;           /**< Tail index for the ring buffer. */
    struct iocb* iocb_queue_head; /**< Pointer to the head of the IOCB queue. */
    struct iocb* iocb_queue_tail; /**< Pointer to the tail of the IOCB queue. */
} dcb;

/** 
 * @brief Global variable indicating if processes have been initialized.
 */
extern int processes_initialized;

/**
 * @brief Serial interrupt service routine.
 */
extern void serial_isr(void);

/** 
 * @brief Array of DCB pointers for each serial device.
 */
extern dcb* dcb_array[4];

/**
 * @brief Validates the specified device.
 * 
 * @param dev The device to validate.
 * @return Non-zero if the device is valid; otherwise, 0.
 */
int isValidDevice(device dev);

/**
 * @brief Retrieves the DCB index for a specified device.
 * 
 * @param dev The device to retrieve the index for.
 * @return Index of the DCB in the array, or -1 if the device is invalid.
 */
int get_dcb_index(device dev);

/**
 * @brief Opens the specified serial device with the given speed.
 * 
 * @param dev The device to open.
 * @param speed The baud rate for the device.
 * @return SUCCESS if successful, or an appropriate error code.
 */
int serial_open(device dev, int speed);

/**
 * @brief Closes the specified serial device.
 * 
 * @param dev The device to close.
 * @return SUCCESS if successful, or an appropriate error code.
 */
int serial_close(device dev);

/**
 * @brief Reads data from the specified serial device.
 * 
 * @param dev The device to read from.
 * @param buf Pointer to the buffer to store the data.
 * @param len Number of bytes to read.
 * @return Number of bytes read, or an error code.
 */
int serial_read(device dev, char* buf, size_t len);

/**
 * @brief Writes data to the specified serial device.
 * 
 * @param dev The device to write to.
 * @param buf Pointer to the buffer containing data to write.
 * @param len Number of bytes to write.
 * @return SUCCESS if successful, or an appropriate error code.
 */
int serial_write(device dev, char* buf, size_t len);

/**
 * @brief Handles serial input interrupts for a specified DCB.
 * 
 * @param dcb Pointer to the DCB handling the input interrupt.
 */
void serial_input_interrupt(struct dcb* dcb);

/**
 * @brief Handles serial output interrupts for a specified DCB.
 * 
 * @param dcb Pointer to the DCB handling the output interrupt.
 */
void serial_output_interrupt(struct dcb* dcb);

/**
 * @brief Handles serial device interrupts.
 */
void serial_interrupt(void);

/**
 * @brief Processes a received character for a specified DCB.
 * 
 * @param received_char The character received.
 * @param dcb Pointer to the DCB handling the character.
 */
void serial_handling(char received_char, struct dcb* dcb);

#endif // SERIAL_DRIVER_H
