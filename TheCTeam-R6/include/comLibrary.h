/**
 * @file comLibrary.h
 * @brief Header file for command library functions in the operating system.
 *
 * This file contains the declarations of functions used for handling
 * user commands.
 * It includes functions for displaying help information, executing user commands,
 * showing version information, and initiating system shutdown.
 */


#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#include <mpx/serial.h>
#include <processes.h>
#include <sys_req.h>
#include <mpx/io.h>
#include <mpx/interrupts.h>
#include <pcbUser.h>

/**
 * @brief Displays help information about available commands.
 *
 * This function outputs a comprehensive help guide with a brief explanation
 * of the commands available to the user, including shutdown, version, date commands,
 * process control block (PCB) commands, and show commands. It provides usage examples
 * and highlights commands with color for better readability.
 */
void help(void);


/**
 * @brief Displays the current version and build date of the system.
 *
 * This function prints the current version of the program and the date of the
 * final implementation. It is useful for users to know the build they are using.
 */
void version(void);

/**
 * @brief Initiates the shutdown process with user confirmation.
 *
 * This function prompts the user for confirmation before initiating a shutdown.
 * It displays a confirmation message and returns 1 to signal that the shutdown
 * confirmation process has been initiated.
 *
 * @return 1 indicating that shutdown confirmation has been initiated.
 */
int shutdown_init(void);

/**
 * @brief Displays help information specifically about date commands.
 *
 * This function provides detailed help information for the date-related commands,
 * including how to get the current date and time, and how to set the system date and time.
 * It includes usage examples and formats.
 */
void help_date(void);

/**
 * @brief Displays help information specifically about PCB commands.
 *
 * This function provides detailed help information for the process control block (PCB)
 * related commands, including how to create, delete, suspend, block, unblock, resume processes,
 * and set process priorities. It includes usage instructions and highlights commands with color.
 */
void help_PCB(void);

/**
 * @brief Displays help information specifically about show commands.
 *
 * This function provides detailed help information for the show commands,
 * including how to display information about specific PCBs, the ready queue,
 * the blocked queue, and all queues. It highlights commands with color for readability.
 */
void help_show(void);

/**
 * @brief Displays help information specifically about memory commands.
 *
 * This function provides detailed help information for memory-related commands,
 * including "Memory Allocate" and "Memory Free." It describes how to create a 
 * memory control block with a specified size and how to free previously allocated 
 * memory. Usage instructions are provided, with examples of each command and 
 * explanations of command parameters.
 */
void help_memory(void);

void help_load(void);

