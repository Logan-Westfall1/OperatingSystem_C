#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include <mpx/serial.h>
#include <processes.h>
#include <sys_req.h>
#include <comHandler.h>
#include <mpx/io.h>
#include <mpx/interrupts.h>
#include <pcbUser.h>
#include "date.h"



#define MAX_ARGS 10 //Maximum number of arguments to take in, arbitrarily chosen

/**
 * @brief Shuts down the system with user confirmation.
 *
 * This function prompts the user for confirmation before initiating a shutdown.
 * If the user confirms with 'y', it returns 1 to signal the shutdown. If cancelled, it returns 0.
 *
 * @param buf The buffer to store user input.
 * @return 1 if shutdown is confirmed, 0 otherwise.
 */
int shutdown_init(void)
{
	sys_req(WRITE, COM1, "Confirm Shutdown? Y/n: \n", 24);
	return 1;
}



/**
 * @brief Displays help information about available commands.
 *
 * This function outputs a help guide with a brief explanation of the commands
 * available to the user, including shutdown, version, and date commands.
 */
void help(void)
{

	print("\n============================================================\n\t\t      \x1b[33mHelp Commands\x1b[0m\n============================================================= \nUse a space as a delimiter between commands.\nExample: 'Help Date' For more information on date commands.\n============================================================= \n");
	

	print_help(0,2, "Shutdown", "Asks for confirmation and exits the program.");
	print_help(0,2, "Version", "Displays the current build version along with the build date.");
	print_help(0,2, "Clear", "Clears the terminal.");
	print_help(0, 2, "Alarm", "Creates an alarm that reads a message out at a certain time. Usage: 'alarm create <time> <message> where time is in 00:00:00 format.");
	print_help(1, 3, "Date", "Get", "Set");
	print_help(1, 5, "Pcb", "Delete", "Suspend", "Resume", "Priority");
	print_help(1, 7, "Show", "PCB", "Ready", "Blocked","Free", "Allocated", "All");
	print_help(1, 5, "Load", "Load R3", "Load R3 Priority", "Load R3 Suspended", "Load R3 Suspended Priority");
}

void help_date(void)
{
	print("\n=============================================================\n\t\t      \x1b[33mDate Commands\x1b[0m\n=============================================================\n");
	
	print_detHelp(3, "Date Get", "Prints the current date and time in MM-DD-YY HH:MM:SS format.", "Usage: 'date get'");
	print_detHelp(5, "Date Set", "Sets both the system date and time using MM-DD-YY HH:MM:SS format.", "Usage: 'date set MM-DD-YY HH:MM:SS'","All values must be numerical and valid date entries","Example:'date set 10-04-24 05:24:46'");

}

//These need to be fleshed out more but we dont have the commands working yet
void help_PCB(void)
{
	print("=============================================================\n\t\t      \x1b[33mPCB Commands\x1b[0m\n=============================================================\n");
	print_detHelp(4, "PCB Delete", "Deletes an existing PCB.", "Usage: 'pcb delete <name>'", "name: The name of the process set during creation");
	print_detHelp(4, "PCB Suspend", "Suspends a given PCB.", "Usage: 'pcb suspend <name>'", "name: The name of the process set during creation");
	print_detHelp(4, "PCB Resume", "Resumes a given PCB.", "Usage: 'pcb resume <name>'", "name: The name of the process set during creation");
	print_detHelp(5, "PCB Priority", "Sets the priority of a given PCB.", "Usage: 'pcb priority <name> <priority>'", "name: The name of the process set during creation", "priority: number 0-9, lower is higher priority");
	
}


//These probably to be fleshes out more but we dont have the commands working yet
void help_show(void)
{
	print("=============================================================\n\t\t      \x1b[33mShow Commands\x1b[0m\n=============================================================\n");

	print_detHelp(4, "Show PCB", "Shows a given PCBs information.", "Usage: 'show pcb <name>'", "name: The name of the process set during creation");
	print_detHelp(3, "Show Ready", "Shows the information of all PCBs in the ready queues", "Usage: 'show ready'");
	print_detHelp(3, "Show Blocked", "Shows the information of all PCBs in the blocked queues", "Usage: 'show blocked'");
	print_detHelp(3, "Show All", "Shows the information of all PCBs in all queues", "Usage: 'show all'");
	print_detHelp(3, "Show Allocated", "Shows all the allocated memory addresses as hexadecimal", "Usage: 'show allocated'");
	print_detHelp(3, "Show Free", "Shows all the free memory addresses as hexadecimal", "Usage: 'show free'");

}


/**
 * @brief Displays the current version and build date of the system.
 *
 * This function prints the version of the program and the final implementation date.
 */

void version(void)
{
	println("The current version of 'The C Team' is the R6 build.");
	println("The date of final implementation was 12/5/24.");
}

void help_load(void)
{
	print("=============================================================\n\t\t      \x1b[33mLoad Commands\x1b[0m\n=============================================================\n");
	print_detHelp(3, "Load R3", "Loads the given R3 processes with a default priority of 1", "Usage: 'load r3'");
	print_detHelp(4, "Load R3 Priority", "Loads the given R3 processes with the inputted priority", "Usage: 'load r3 priority <priority>'", "priority: The priority you want all of the processes to be loaded as");
	print_detHelp(3, "Load R3 Suspended", "Loads the given R3 processes as suspended with a default value of 1. Must unsuspend to run", "Usage: 'load r3 suspended'");
	print_detHelp(4, "Load R3 Suspended Priority", "Loads the given R3 processes as suspended with the inputted priority", "Usage: 'load r3 suspended priority <priority>'", "priority: The priority you want all of the processes to be loaded as");
}
