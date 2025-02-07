target remote localhost:1234
cd
cd TheCTeam
symbol kernel.bin
tui enable
define pcb
	print *(struct pcb*) $arg0
end
define d
	print *(struct dcb*) dcb_array[0]
end
define i
	print *(struct iocb*) $arg0
end
define c
	print *(struct context*) $arg0
end
define vals			
	printf "Ready Head: "
	if(ready_head)	
		p ready_head->name
	else
		printf "null\n"
	end
	printf "Blocked Head: "
	if(blocked_head)
		p blocked_head->name
	else
		printf "null\n"
	end	
	printf "Current Process: "
	if(current_process)
		p current_process->name
	else
		printf "null\n"
	end
end
define combreaks
	printf "serial_interrupts.c\n"
	printf "serial_interrupt 526\n"
	printf "comLibrary.c\n"
	printf "help 44\n"
end	
define m
	print *(struct pcb*) current_process
end
define n
	print *(struct pcb*) 0xd000158
end
break serial_interrupts.c:429
continue
