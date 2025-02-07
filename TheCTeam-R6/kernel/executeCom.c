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
#include <comLibrary.h>
#include <load_r3.h>
#include <alarm.h>
#include <memUser.h>


#define MAX_ARGS 10 //Maximum number of arguments to take in, arbitrarily chosen



/**
 * @brief Parses and executes the given command.
 *
 * This function takes a user command string, tokenizes it to extract the command and arguments,
 * and executes the corresponding functionality based on the command (shutdown, help, date, version).
 *
 * @param buf The input command string from the user.
 * @return 0 on successful execution, 1 if shutdown is confirmed.
 */
int SHUTDOWN_INIT = 0;
int execute_com(char* buf)
{
	if (buf[0] == '\0')
	{
		return 0;
	}
	toLower(buf); //convert all characters in buffer to be lowercase for easier comparisons

	char* args[MAX_ARGS] = { 0 }; //Character array to contain arguments of MAX_ARGS size
	int argc = 0; //Represents argument count, will be increased as strtok splits arguments


	//Tokenize input to detect command and then following arguments
	char* token = strtok(buf, " ");
	while (token != NULL && argc < MAX_ARGS)
	{
		args[argc++] = token;
		token = strtok(NULL, " ");
	}

	//Checks for if one command is entered
	if (argc == 0) {
        print_e("Error: No command entered");
        return 0;
    }

	//Run if statements against defined commands
	if (!strcmp(args[0], "shutdown"))
	{
		SHUTDOWN_INIT = shutdown_init();
	}
	else if (SHUTDOWN_INIT && !strcmp(args[0], "y"))
	{
		sys_req(WRITE, COM1, "Shutdown confirmed...\n", 23);
		SHUTDOWN_INIT = 0;
		return 1;
	}
	else if (SHUTDOWN_INIT && !strcmp(args[0], "n"))
	{
		sys_req(WRITE, COM1, "Shutdown cancelled...\n", 23);
		SHUTDOWN_INIT = 0;
	}
	else if (!strcmp(args[0], "help"))
	{
		if(args[1] != 0)
		{
			if(!strcmp(args[1], "date"))
			{
				help_date();
			}
			else if(!strcmp(args[1], "pcb"))
			{
				help_PCB();
			}
			else if(!strcmp(args[1], "show"))
			{
				help_show();
			}
			else if( !strcmp(args[1], "load"))
			{
				help_load();
			}
			else
			{
				print_e("Error: Invalid Help command entered");
			}
		}
		else
		{
			help();
		}
	}
	else if (!strcmp(args[0], "date"))
	{
		if (args[1] != 0)
		{
			if (!strcmp(args[1], "set"))
			{
				set_date(args[2], args[3]);
			}
			else if (!strcmp(args[1], "get"))
			{
				if (args[2] != NULL)
				{
					get_time();
				}
				else get_date();
			}
			else
			{
				print_e("Error: Invalid Date command entered");
			}
		}
		else
		{
			print_e("Error: Invalid Date command entered");
		}
	}
	else if (!strcmp(args[0], "version"))
	{
		version();
	
	}
	//not sure if this is correct or not, just tried to get a skeleton
	else if (!strcmp(args[0], "pcb"))
	{
		if(args[1] != 0)
		{			
			if (!strcmp(args[1], "delete"))
			{
				if(argc != 3)
				{
					print_e("Error: Incorrect usage of delete. Usage: 'pcb delete <name>'. Use 'help pcb' for more info");
					return 0;
				}
				deletePCB(args[2]);
			}
			else if (!strcmp(args[1], "suspend"))
			{
				if(argc != 3)
				{
					print_e("Error: Incorrect usage of suspend. Usage: 'pcb suspend <name>'. Use 'help pcb' for more info");
					return 0;
				}
				suspendPCB(args[2]);
			}
			else if (!strcmp(args[1], "resume"))
			{
				if(argc != 3)
				{
					print_e("Error: Incorrect usage of resume. Usage: 'pcb resume <name>'. Use 'help pcb' for more info");
					return 0;
				}
				resumePCB(args[2]);
			}
			else if(!strcmp(args[1], "priority"))
			{
				if(argc != 4)
				{
					print_e("Error: Incorrect usage of priority. Usage: 'pcb priority <name> <priority>'. Use 'help pcb' for more info");
					return 0;
				}
				setPCBPriority(args[2], atoi(args[3]));
			}
			else
			{
				print_e("Error: Invalid PCB command entered");
			}

		}
		else
		{
			print_e("Error: Invalid PCB command entered");
		}
	}
	else if (!strcmp(args[0], "show"))
	{
		if(args[1] != 0)
		{
			if (!strcmp(args[1], "pcb"))
			{
				if(args[2] != 0)
				{
					showPCB(args[2]);
				}
				else
				{
					print_e("Error: Incorrect usage of Show. Usage: 'show pcb <name>'. Use 'help show' for more info");

				}
			}
			else if(!strcmp(args[1], "ready"))
			{
				showReady();
			}
			else if(!strcmp(args[1], "blocked"))
			{
				showBlocked();
			}
			else if(!strcmp(args[1], "all"))
			{
				showAll();
			}
			else if(!strcmp(args[1], "allocated"))
			{
				show_allocated_memory();
			}
			else if(!strcmp(args[1], "free"))
			{
				show_free_memory();
			}
			else
			{
				print_e("Error: Use of Show. Use 'help show' for more information.");
			}
		}
		else
		{
			print_e("Error: Invalid show command entered.");

		}
	}
	else if (!strcmp(args[0], "clear"))
	{
		print("\033[2J\033[H");
	}
	else if (!strcmp(args[0], "load") && !strcmp(args[1], "r3"))
	{
		if (args[2] != NULL && !strcmp(args[2], "suspended"))
		{
			if(args[3] != NULL && !strcmp(args[3], "priority"))
			{
				if(args[4] != NULL)
				{
					int priority = atoi(args[4]);
					//ensures priority is within the range [0, 8]
					if(priority < 9 && priority >= 0)
					{
						char prio_s [2];
						print(GREEN("Loading R3 Suspended With Priority of: "));
						println(itoa(priority, prio_s));	
						load_processes(1, priority);
					}
					else
					{
						print_e("Error: Invalid priority entered. Value must be from 0 to 8");
					}
				}
				else
				{
					print_e("Error: Incorrect usage of load r3 suspended priorty. Use 'help' for more information");
				}
			}
			else
			{
				println(GREEN("Loading R3 Suspended"));
				load_processes(1, 1);
			}
		}
		else if(args[2] != NULL && !strcmp(args[2], "priority"))
		{
			if(args[3] != NULL)
			{
				int priority = atoi(args[3]);
				//ensures priority is within the range [0, 8]
				if(priority < 9 && priority >= 0)
				{
					char prio_s[2];
					print(GREEN("Loading R3 With Priority of: "));
					println(itoa(priority, prio_s));
					load_processes(0, priority);
				}
				else
				{
					print_e("Error: Invalid priority entered. Value must be from 0 to 8");
				}
			}
			else
			{
			print_e("Error: Incorrect usage of load r3 priorty. Use 'help' for more information");
			}
		}
		else
		{
			println(GREEN("Loading R3"));
			load_processes(0, 1);
		}
	}
	else if (!strcmp(args[0], "alarm"))
	{
		if(argc != 4)
		{
			print_e("Error: Invalid alarm command entered");
			return 0;
		}
		if (!strcmp(args[1], "create"))
		{
			if(argc != 4)
			{
				print_e("Error: Incorrect usage of alarm");
				return 0;
			}
			create_alarm(args[2], args[3]);
		}
	}
	else
		{
			print_e("Error: Command not found or does not exist");
		}


	return 0;


}

