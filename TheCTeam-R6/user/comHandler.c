
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include <mpx/serial.h>
#include <processes.h>
#include <sys_req.h>
#include <comHandler.h>
#include <mpx/io.h>
#include <mpx/interrupts.h>
#include <comLibrary.h>
#include <executeCom.h>
#include <pcb.h>
#include <serial_interrupts.h>


#define MAX_COMMAND_LENGTH 100 //Max length of commands, at 100 as that is max buffer size currently using

int SHUTDOWN = 0;








//Basic comhand structure provided to us in the slides, with the addition of execute_com and shutdown
void comhand(void)
{
	char* wait_for_input = YELLOW("The ") CYAN("C ") YELLOW("Team OS: ");
	while (1)
	{	
		
		sys_req(WRITE, COM1, wait_for_input, strlen(wait_for_input));
		char buf[MAX_COMMAND_LENGTH] = { 0 };
		sys_req(READ, COM1, buf, sizeof(buf));
		SHUTDOWN = execute_com(buf);
		if (SHUTDOWN)
		{
			clear_queues();
			sys_req(EXIT);
			return;
		}
		
		sys_req(IDLE);
	}

		
	
}

