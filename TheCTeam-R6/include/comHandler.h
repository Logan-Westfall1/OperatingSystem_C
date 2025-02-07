#include <stddef.h>
/**
 @file comHandler.h
 @brief Kernel function for comhandler
*/

/**
 * @brief Command handler loop for the system.
 *
 * This function continuously reads user input from the serial COM1 port and processes
 * the input by executing the appropriate command using the `execute_com` function.
 * If the shutdown command is executed, the function will exit the loop and return,
 * signaling the shutdown of the system.
 *
 * The function operates indefinitely until the shutdown command is confirmed.
 */
void comhand(void);
