

global serial_isr

extern serial_interrupt 

serial_isr:
    call serial_interrupt
    iret 


