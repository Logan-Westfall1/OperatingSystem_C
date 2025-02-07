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
#include <load_r3.h>
#include <pcbUser.h>






void load_processes(int suspended, int priority)
{
    //Load processes and initialize them
    pcb* process1 = pcb_setup("process1", USER_PROCESS, priority);
    pcb* process2 = pcb_setup("process2", USER_PROCESS, priority);
    pcb* process3 = pcb_setup("process3", USER_PROCESS, priority);
    pcb* process4 = pcb_setup("process4", USER_PROCESS, priority);
    pcb* process5 = pcb_setup("process5", USER_PROCESS, priority);

    //Initialize context
    initialize_context(process1, proc1, suspended);
    initialize_context(process2, proc2, suspended);
    initialize_context(process3, proc3, suspended);
    initialize_context(process4, proc4, suspended);
    initialize_context(process5, proc5, suspended);
}

void initialize_context(pcb* process, void (*function_ptr)(void), int suspended)
{
    if (process != NULL)
    {
        process->stack_pointer = &process->stack[1024 - sizeof(context)];
        struct context* ctx = (struct context*)process->stack_pointer;

        // Initialize context values

        ctx->gs = 0x10;                      // GS segment
        ctx->fs = 0x10;                      // FS segment
        ctx->es = 0x10;                      // ES segment
        ctx->ds = 0x10;                      // Data segment
        ctx->ss = 0x10;                      // Stack segment (was missing before)
        ctx->eax = 0;
        ctx->ebx = 0;
        ctx->ecx = 0;
        ctx->edx = 0;
        ctx->edi = 0;
        ctx->esi = 0;
        ctx->ebp = (unsigned int)process->stack;
        ctx->eip = (unsigned int)function_ptr;
        ctx->cs = 0x08;                      // Code segment
        ctx->eflags = 0x0202;
        ctx->esp = (unsigned int)process->stack_pointer;

        pcb_insert(process);
        if (suspended)
        {
            suspendPCB((char*)process->name);
        }
       
   }

    
}


