#include <mpx/gdt.h>
#include <mpx/interrupts.h>
#include <mpx/serial.h>
#include <mpx/vm.h>
#include <sys_req.h>
#include <string.h>
#include <memory.h>
#include <comHandler.h>
#include <load_r3.h>
#include <pcb.h>
#include <mem_lib.h>
#include <io_scheduler.h>

/**
 @file kernel/kmain.c
 @brief Contains kmain() func that controls system functionality.
*/
static void klogv(device dev, const char* msg)
{
	char prefix[] = "klogv: ";
	serial_out(dev, prefix, strlen(prefix));
	serial_out(dev, msg, strlen(msg));
	serial_out(dev, "\r\n", 2);
}

void kmain(void)
{
	// 0) Serial I/O -- <mpx/serial.h>
	// If we don't initialize the serial port, we have no way of
	// performing I/O. So we need to do that before anything else so we
	// can at least get some output on the screen.
	// Note that here, you should call the function *before* the output
	// via klogv(), or the message won't print. In all other cases, the
	// output should come first as it describes what is about to happen.;
	serial_init(COM1);
	klogv(COM1, "Initialized serial I/O on COM1 device...");

	// 1) Global Descriptor Table (GDT) -- <mpx/gdt.h>
	// Keeps track of the various memory segments (Code, Data, Stack, etc.)
	// required by the x86 architecture. This needs to be initialized before
	// interrupts can be configured.
	klogv(COM1, "Initializing Global Descriptor Table...");
	gdt_init();

	// 2) Interrupt Descriptor Table (IDT) -- <mpx/interrupts.h>
	// Keeps track of where the various Interrupt Vectors are stored. It
	// needs to be initialized before Interrupt Service Routines (ISRs) can
	// be installed.
	klogv(COM1, "Initializing Interrupt Descriptor Table...");
	idt_init();



	// 3) Disable Interrupts -- <mpx/interrupts.h>
	// You'll be modifying how interrupts work, so disable them to avoid
	// crashing.
	klogv(COM1, "Disabling interrupts...");
	cli();

	// 4) Interrupt Request (IRQ) -- <mpx/interrupts.h>
	// The x86 architecture requires ISRs for at least the first 32
	// Interrupt Request (IRQ) lines.
	klogv(COM1, "Initializing Interrupt Request routines...");
	irq_init();

	// 5) Programmable Interrupt Controller (PIC) -- <mpx/interrupts.h>
	// The x86 architecture uses a Programmable Interrupt Controller (PIC)
	// to map hardware interrupts to software interrupts that the CPU can
	// then handle via the IDT and its list of ISRs.
	klogv(COM1, "Initializing Programmable Interrupt Controller...");
	pic_init();

	// 6) Reenable interrupts -- <mpx/interrupts.h>
	// Now that interrupt routines are set up, allow interrupts to happen
	// again.
	klogv(COM1, "Enabling Interrupts...");
	sti();
	// 7) Virtual Memory (VM) -- <mpx/vm.h>
	// Virtual Memory (VM) allows the CPU to map logical addresses used by
	// programs to physical address in RAM. This allows each process to
	// behave as though it has exclusive access to memory. It also allows,
	// in more advanced systems, the kernel to swap programs between RAM and
	// storage (such as a hard drive or SSD), and to set permissions such as
	// Read, Write, or Execute for pages of memory. VM is managed through
	// Page Tables, data structures that describe the logical-to-physical
	// mapping as well as manage permissions and other metadata.
	klogv(COM1, "Initializing Virtual Memory...");
	vm_init();

	// 8) MPX Modules -- *headers vary*
	// Module specific initialization -- not all modules require this.
	klogv(COM1, "Initializing MPX modules...");
	// R5: sys_set_heap_functions(...);
	size_t size = 50000;
	initialize_heap(size);
	sys_set_heap_functions(allocate_memory, free_memory);
	// R4: create commhand and idle processes

	// 9) YOUR command handler -- *create and #include an appropriate .h file*
	// Pass execution to your command handler so the user can interact with
	// the system.
	serial_open(COM1, 19200);
	klogv(COM1, "Transferring control to commhand...");
	char* intro1 = " ██████╗    ████████╗███████╗ █████╗ ███╗   ███╗     ██████╗ ███████╗\n██╔════╝    ╚══██╔══╝██╔════╝██╔══██╗████╗ ████║    ██╔═══██╗██╔════╝\n██║            ██║   █████╗  ███████║██╔████╔██║    ██║   ██║███████╗\n██║            ██║   ██╔══╝  ██╔══██║██║╚██╔╝██║    ██║   ██║╚════██║\n╚██████╗       ██║   ███████╗██║  ██║██║ ╚═╝ ██║    ╚██████╔╝███████║\n ╚═════╝       ╚═╝   ╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝     ╚═════╝ ╚══════╝\n";
	///*char* intro2 = "\n██╔════╝    ╚══██╔══╝██╔════╝██╔══██╗████╗ ████║    ██╔═══██╗██╔════╝";
	//char* intro3 = "\n██║            ██║   █████╗  ███████║██╔████╔██║    ██║   ██║███████╗";
	//char* intro4 = "\n██║            ██║   ██╔══╝  ██╔══██║██║╚██╔╝██║    ██║   ██║╚════██║";
	//char* intro5 = "\n╚██████╗       ██║   ███████╗██║  ██║██║ ╚═╝ ██║    ╚██████╔╝███████║";*/
	//char* intro6 = "\n ╚═════╝       ╚═╝   ╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝     ╚═════╝ ╚══════╝\n";
	char* help_introduction ="=============================================================\nRunning 'The "CYAN("C")" Team', Type 'help' for information on commands\n=============================================================\n";
	sys_req(WRITE, COM1, intro1, strlen(intro1));
	/*sys_req(WRITE, COM1, intro2, strlen(intro2));
	sys_req(WRITE, COM1, intro3, strlen(intro3));
	sys_req(WRITE, COM1, intro4, strlen(intro4));
	sys_req(WRITE, COM1, intro5, strlen(intro5));
	sys_req(WRITE, COM1, intro6, strlen(intro6));*/

	sys_req(WRITE, COM1, help_introduction, strlen(help_introduction));
	
	
	pcb* comhandler = pcb_setup("comhand", SYSTEM_PROCESS, 0);
	initialize_context(comhandler, comhand, 0);

	pcb* sysidle = pcb_setup("sysidle", SYSTEM_PROCESS, 9);
	initialize_context(sysidle, sys_idle_process, 0);
	processes_initialized = 1;
	
	__asm__ volatile ("int $0x60" :: "a"(IDLE));

	// 10) System Shutdown -- *headers to be determined by your design*
	// After your command handler returns, take care of any clean up that
	// is necessary.
	klogv(COM1, "Starting system shutdown procedure...");

	// 11) Halt CPU -- *no headers necessary, no changes necessary*
	// Execution of kmain() will complete and return to where it was called
	// in boot.s, which will then attempt to power off Qemu or halt the CPU.
	klogv(COM1, "Halting CPU...");

}
