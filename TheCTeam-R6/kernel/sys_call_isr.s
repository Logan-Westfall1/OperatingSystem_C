
global sys_call_isr

extern sys_call			; The C function that sys_call_isr will call
sys_call_isr:;
	push ebp;
	push esi;
	push edi;
	push edx;
	push ecx;
	push ebx;
	push eax;
	push ss;
	push ds				;
	push es				;
	push fs				;
	push gs				;
	push esp;

	call sys_call		; Call C function

	mov esp, eax		; Set ESP to the return value of the C function

	pop gs				; restore segment registers
	pop fs				;
	pop es				;
	pop ds				;
	pop ss;
	pop eax;
	pop ebx;
	pop ecx;
	pop edx;
	pop edi;
	pop esi;
	pop ebp;

	iret				; Return
