
#include <serial_interrupts.h>


/*
* To make it clearer where I am getting values and stuff from I have included references to the documents
* she provided us. r6 refers to the powerpoint, r6d refers to the detailed document. 
* Ex. r6d_13 means page 13 of the r6 detailed document.
*/

#define PIC_MASK_PORT 0x21
#define KEY_BACKSPACE  0x08
#define MAC_DELETE 0x7F
#define WIN_DELETE 0x2E
#define KEY_CARRIAGE_RETURN 0x0D // \r
#define KEY_NEWLINE 0x0A // \n


dcb* dcb_array[4] = { 0 };
int processes_initialized = 0;




int get_dcb_index(device dev) {
    switch (dev) {
    case 0x3F8: // COM1
        return 1;
    case 0x2F8: // COM2
        return 2;
    case 0x3E8: // COM3
        return 3;
    case 0x2E8: // COM4
        return 4;
    default:
        return -1; // Invalid device
    }
}

unsigned short getDev(int dev)
{
    switch (dev)
    {
    case 1: // COM1
    case COM1:
        return 0x3F8;
    case 2: // COM2
    case COM2:
        return 0x2F8;
    case 3: // COM3
    case COM3:
        return 0x3E8;
    case 4: // COM4
    case COM4:
        return 0x2E8;
    default:
        return -1; // Invalid device
    }
    
}

int isValidDevice(device dev)
{
    return (dev == 0x3f8 || dev == 0x2f8 || dev == 0x3e8 || dev == 0x2e8);
}



/*
* 1. Ensure that the parameters are valid, and that the device is not currently open. DONE
* 
* 2. Initialize the DCB. In particular, this should include indicating that the device is open, setting the
* event flag to 0, and setting the initial device status to idle. In addition, the ring buffer parameters
* must be initialized. DONE
* 
* 3. Install the new handler in the interrupt vector. DONE
* 
* 4. Compute the required baud rate divisor.DONE
* 
* 5. Store the value 0x80 in the Line Control Register. This allows the first two port addresses to access
* the Baud Rate Divisor register. DONE
* 
* 6. Store the high order and low order bytes of the baud rate divisor into the MSB and LSB registers,
* respectively. DONE
* 
* 7. Store the value 0x03 in the Line Control Register. This sets the line characteristics to 8 data bits, 1
* stop bit, and no parity. It also restores normal functioning of the first two ports. DONE
* 
* 8. Enable the appropriate level in the PIC mask register.
* 
* 9. Enable overall serial port interrupts by storing the value 0x08 in the Modem Control register.
* 
* 10. Enable input ready interrupts only by storing the value 0x01 in the Interrupt Enable register.
*/

enum registers
{
    IER = 1,
    IIR = 2,
    LCR = 3,
    MCR = 4,
    LSR = 5,
    MSR = 6,
};
int serial_open(device dev, int speed)
{
    if (!isValidDevice(dev))
    {
        return ERR_INVALID_DEVICE;
    }
    if (speed <= 0)
    {
        return ERR_INVALID_BAUD_DIVISOR;
    }

    dcb* dcb = sys_alloc_mem(sizeof(struct dcb));
    dcb_array[get_dcb_index(dev) - 1] = dcb;
    dcb->device_id = dev;
    if (dcb->open == 1)
    {
        return ERR_PORT_ALREADY_OPEN;
    }

    //Initialize DCB
    dcb->open = 1;
    dcb->event_flag = 0;
    dcb->status = DCB_IDLE; //0
    dcb->ring_head = dcb->ring_tail = 0; //Set ring head and ring tail to 0 bc buffer is empty
    dcb->iocb_queue_head = dcb->iocb_queue_tail = NULL;

    unsigned short base = dev;  //Assign base to the base IO of the device

    idt_install(0x24, (void(*)(void*))serial_isr); //Installs ISR into the Interrupt Descriptor Table <mpx/interrupts.h>

    //Figure out baud rate r6d_12 and r6d_13
    int divisor = 115200 / speed; //Calc divisor for desired baud rate
    if (divisor < 0 || divisor > 0xFFFF)
    {
        return ERR_INVALID_BAUD_DIVISOR;
    }


    /*
    * Register Values:
    * Base:
    * +1 - I/O Interrupt Enable Register
    * +2 - Interrupt ID Register
    * +3 - Line Control Register
    * +4 - Modem Control Register
    * +5 - Line Status Register
    * +6 - Modem Status Register
    * IF BIT 7 OF the Line Control Register is set to 1, this CHANGES
    * Base and Base+1 are attched to LSB and MSB of the Baud Rate Divisor (BRD)
    * 
    */
    outb(base + IER, 0x00);
    (void)inb(base);
    cli();
    outb(base + LCR, 0x80); //Line Control Register 0x80 = 10000000, Flips Base and Base+1 to BRD to assign
    outb(base, divisor & 0xFF); //Set low byte of divisor
    outb(base + IER, (divisor >> 8)); //Set high byte of divisor
    outb(base + LCR, 0x03); //Line Control Register 0x03 = 00000011, Restores first two ports functionality, Sets line characteristics to 8 bits, 1 stop bit, no parity

    //Enable serial input interrupts
   //outb(base + 1, 0x01); //Enable received data available interrupt, IER 00000001
    
    int mask = inb(PIC_MASK_PORT); //Read the current interrupt mask
    mask &= ~(1 << (3 + (get_dcb_index(dev) % 2))); //Enable IRQ (Interrupt requests) for this serial port
    /* Mask Values for Com1: Correct
    * Initial Mask: 184, 10111000
    * Modified Mask: 168,10101000 
    */
    /*
    * Line above looks insane so here is an explanation:
    * COM1 and COM3 share IRQ4, COM2 and COM4 share IRQ3.
    * dev % 2 : Calcs whether device is even or odd
    * 3 + dev % 2 : Determines IRQ line associated with serial port. 
    * Ex. if dev = 1 (COM1), results is 3 + 1 = 4. if dev = 2 (COM2), result is 3 + 0 = 3.
    * 1 << (4 + (dev % 2)) : Bitmask so only bit corresponding to calculated IRQ is set to 1.
    * Ex. For IRQ4, 1 << 4 = 0x00010000. For IRQ3, 1 << 5 = 0b00100000.
    * ~ : Inverts the bitmask, so bit for target IRQ is 0 and all other bits are 1.
    * &= Basically undoes the ~ except for the bit we care about here, a.k.a it ensures all other bits remain unchanged
    * Full Example for COM1 (dev = 1)
    * dev % 2 = 1;
    * 4 + 1 = 5;
    * 1 << 5 = 0b00100000;
    * ~(0b00010000) = 0b11101111;
    * mask &= 0b11101111; Enables IRQ4 for COM1. 4th bit (starting from 0 on the right) is now 0. 
    * NO idea if this will actually work!
    */
    
    outb(PIC_MASK_PORT, mask); //Write updated mask to PIC
    outb(0x20, 0x20);
    

    outb(base + MCR, 0x08); //Enable interrupts in modem control register r6d14
    outb(base + IER, 0x01); //Enable received data available interrupt r6d14
    sti();
    return SUCCESS;

}



int serial_close(device dev)
{
    if (!isValidDevice(dev))
    {
        return ERR_INVALID_DEVICE;
    }

    dcb* dcb = dcb_array[dev - 1];
    if (!dcb->open)
    {
        return ERR_PORT_NOT_OPEN;
    }

    unsigned short base = dev;

    dcb->open = 0; //Mark port as closed

    int mask = inb(PIC_MASK_PORT); //Read current interrupt mask
    mask |= (1 << (4 + (dev % 2))); //Disable IRQ for this serial port
    /*
    * Similar to the long explanation from before, |= is bitwise OR operator, so this 
    * basically just sets the bit at the right position to 1 and doesn't change the other values. r6_17 
    */
    outb(PIC_MASK_PORT, mask); //Write updated mask back to PIC

    outb(base + 4, 0x00); //Disable all modem control interrupts
    outb(base + 1, 0x00); //Disable all serial interrupts

    return SUCCESS; 

}


int serial_read(device dev, char* buf, size_t len)
{
    if (!isValidDevice(dev))
    {
        return ERR_INVALID_DEVICE;
    }
  
    if (!buf)
    {
        return ERR_INVALID_BUFFER_ADDRESS;
    }
    
    if (len == 0)
    {
        return ERR_INVALID_COUNT;
    }

    dcb* dcb = dcb_array[get_dcb_index(dev) - 1];
    if (!dcb->open) 
    {
        return ERR_PORT_NOT_OPEN;
    }

    if (dcb->status != DCB_IDLE)
    {
        return ERR_DEVICE_BUSY;
    }

    dcb->input_buffer = buf;
    dcb->input_size = len;
    dcb->status = DCB_READING; //DCB_READING (1) status
    dcb->event_flag = 0; //Clear caller's event flag

    size_t count = 0;

    cli(); //Disable interrupts to prevent concurrent access to ring buffer

    //Copy characters from the ring buffer to the requestor's buffer
    /*
    * While loop explanation:
    * While logic:
    * count < len : Ensures number of bytes read (count) does not exceed requested length (len)
    * && dcb->ring_head != dcb->ring_tail : Ensures there are still characters in the ring buffer to read
    * end While logic
    * 
    * buf[count] = dcb->ring_buffer[dcb->ring_tail] : Copies a character from the ring buffer to the user's buffer
    * : ring_tail points to the next character in the ring buffer to be read
    * : The character at this position is copied into buf[count], where count tracks how many characters have been read thus far
    * 
    * dcb->ring_tail = (dcb->ring_tail + 1) % sizeof(dcb->ring_buffer) : Advance the ring_tail pointer
    * : After reading a character, ring_tail is incremented by 1 to point to the next character
    * : (dcb->ring_tail + 1) % sizeof(dcb->ring_buffer) : Calculation ensures ring_tail wraps around when it reaches end of buffer
    * : Modulus % is used to keep ring_tail within the bounds of the ring buffer's size
    * : This makes the ring_buffer a circular buffer
    * 
    * If logic: 
    * buf[count - 1] == '\n' : Checks if last character read into buffer is a newline, break if so
    */
    while (count < len && dcb->ring_head != dcb->ring_tail)
    {
        if (dcb->ring_buffer[dcb->ring_tail] == '\r' || dcb->ring_buffer[dcb->ring_tail] == '\n')
        {
            break;
        }
        buf[count] = dcb->ring_buffer[dcb->ring_tail];
        dcb->ring_tail = (dcb->ring_tail + 1) % sizeof(dcb->ring_buffer);
        count++;
        /*if (buf[count - 1] == '\n' || buf[count - 1] == '\r')
        {
            break;
        }*/
    }
   
    sti(); //Re-enable interrupts

    /*
    * If the ring buffer does not contain enough characters to fulfill the request, then more reading
    * must be done via the ISR as new characters arrive.
    * 
    * If logic:
    * count < len : If count < len, this means the number of characters already read from the ring buffer
    * is less than the requested number len
    * Basically, the user buffer is not full yet, and we need to read more characters into it
    * 
    * dcb->input_buffer += count : Moves the pointer forward by count characters, so the next characters
    * will be placed correctly
    * 
    * dcb->input_size -= count : input_size keeps track of how many characters are left to be read
    * Since count characters have been read, we need to subtract count from the input_size.
    * 
    * return SUCCESS : Since there are more characters to be read, we return SUCCESS to allow the ISR
    * to handle the continuation of reading. When new data arrives, it will write directly to the remaining
    * part of the user buffer (dcb->input_buffer) until either the buffer is full or the requested number of 
    * characters (len) has been received
    */
    if (count < len && buf[count - 1] != '\n' && buf[count - 1] != '\r') //If more characters are needed, return
    {
        dcb->input_buffer += count;
        dcb->input_size -= count;
        return SUCCESS; //Reading will continue via ISR
    }

    //If block is complete, reset DCB status to DCB_IDLE and set event flag to indicate completion
    dcb->status = DCB_IDLE; //0
    dcb->event_flag = EVENT_FLAG_SET; //1

    return count;

}

int serial_write(device dev, char* buf, size_t len)
{
    //Normal validity checks
    if (!isValidDevice(dev))
    {
        return ERR_INVALID_DEVICE;
    }

    if (!buf)
    {
        return ERR_INVALID_BUFFER_ADDRESS;
    }

    if (len == 0)
    {
        return ERR_INVALID_COUNT;
    }

    dcb* dcb = dcb_array[get_dcb_index(dev) - 1];
    if (!dcb->open)
    {
        return ERR_PORT_NOT_OPEN;
    }

    if (dcb->status != DCB_IDLE)
    {
        return ERR_DEVICE_BUSY;
    }

    
    cli();
    dcb->output_buffer = buf; //Set the dcb's output buffer to point to the passed in buffer
    dcb->output_size = len; //Set output_size to equal requested write length
    dcb->status = DCB_WRITING; //2
    dcb->event_flag = EVENT_FLAG_CLEAR; //0
    sti();

    unsigned short base = dev;
    if (!processes_initialized)
    {
        while (len--)
        {
            while (!(inb(base + 5) & 0x20));
            outb(base, *buf++);
        }
        dcb->status = DCB_IDLE;  // Set status to DCB_IDLE
        dcb->event_flag = EVENT_FLAG_SET;  // Indicate writing completion
        return SUCCESS;
    }

    if (len > 0)
    {
        outb(base, dcb->output_buffer[0]); //Write the first character to the serial port
        dcb->output_buffer++; //Move the pointer to the next character
        dcb->output_size--; //Decrement the size of the buffer
        if (dcb->output_size == 0) {
            dcb->status = DCB_IDLE;  // Set status to DCB_IDLE
            dcb->event_flag = EVENT_FLAG_SET;  // Indicate writing completion
            return SUCCESS;
        }
    }

    //Enable output interrupts to continue writing the rest of the data
    unsigned char ier = inb(base + 1); //Read the current interrupt enable register (ier)
    /*
    * Write back to the IER
    * ier | 0x02 : Bitwise OR to set specific bit in IER while keeping the rest of the bits unchanged
    * 0x02 is hexadecimal value representing a bitmask where only the second bit (bit 1) is set to 1. Binary: 00000010
    * Basically just sets bit 1 of the IER to while not changing the others
    */
    outb(base + 1, ier | 0x02); //Enable transmitter holding register empty interrupt

    return SUCCESS; //Writing will continue via ISR
}







void serial_input_interrupt(struct dcb* dcb)
{
    /*
    * Determine base IO of device
    * dcb - dcb_array : This should work because pointer artihmetic betweeen two pointers that point to
    * elements of the same array results in the offset between them
    * Ex. if dcb points to the dcb for COM3, dcb - dcb_array would be 2.
    * + 1 : DCB array is indexed from 0, so we need to add one for the right device
    *
    * While logic:
    * : inb(base+5) : Reads the LSR
    * : & 0x01 : Bitwise to make sure bit 0 is 1, indicating there is data available to read
    *
    * : char received_char = inb(base) : Reads a character from the base address
    * : size_t next_head = (dcb->ring_head + 1) & sizeof(dcb->ring_buffer) : Like earlier, calculates
    * position of ring_head and wraps around if necessary
    *
    * If logic:
    * : next_head != dcb->ring_tail : Checks if the ring buffer is not full
    * : dcb->ring_buffer[dcb->ring_head] = received_char : Stores received character in ring buffer
    * : dcb->ring_head = next_head : Moves head pointer to next position
    * end If;
    *
    * If logic:
    * : dcb->status = DCB_READING && dcb->input_size > 0 : Checks if device is reading, and if there is space
    * left in the input buffer
    *
    * : *dcb->input_buffer++ = received_char : Copies received character directly to user's input buffer
    * Pointer is incremented so that next character will be written to the next position
    * : dcb->input_size-- : Decreases remaining size of input buffer to show character has been stored
    *
    * If logic:
    * : received_char == '\n' || dcb->input_size == 0 : Checks if received character is a newline, or if
    * input buffer is full
    * : dcb->status = DCB_IDLE : Sets status to idle since read operation is completed
    * : dcb->event_flag = EVENT_FLAG_SET : Sets event flag to indicate completion
    * end If;
    * end If;
    * end While;
    *
    */
    cli();
    unsigned short base = getDev(dcb->device_id);
    static int escape_state = 0;
    static char escape_buffer[3];
    static int escape_index = 0;
    static int index = 0;
    static int input_count = 0;
    char received_char;
    size_t next_head;
    int i;
    while (inb(base + 5) & 0x01) // While data is available in the Receiver Buffer
    {
        received_char = inb(base);
        if (escape_state > 0)
        {
            escape_buffer[escape_index++] = received_char;
            if (escape_state == 1 && received_char == '[')
            {
                escape_state = 2;
            }
            else if (escape_state == 2)
            {
                if (received_char == 'A') {
                    // Up arrow key
                    
                }
                else if (received_char == 'B') {
                    // Down arrow key
                    
                }
                else if (received_char == 'C') {
                    // Right arrow key
                    if (index < input_count)
                    {
                        outb(base, dcb->input_buffer[index]);
                        index++;
                    }
                }
                else if (received_char == 'D') {
                    
                    // Left arrow key
                    if (index > 0)
                    {
                        index--;
                        outb(base, '\b');
                    }
                }
                escape_state = 0;
                escape_index = 0;
                continue;
            }
        }
        else if (received_char == '\033')
        {
            escape_state = 1;
            escape_index = 0;
            escape_buffer[escape_index++] = received_char;
            continue;
        }
        else if (received_char == KEY_BACKSPACE || received_char == MAC_DELETE)
        {
            if (index > 0)
            {
                index--;
                for (i = index; i < input_count; i++)
                {
                    dcb->input_buffer[i] = dcb->input_buffer[i + 1];
                }
                input_count--;
                outb(base, '\b');
                for (i = index; i < input_count; i++)
                {
                    outb(base, dcb->input_buffer[i]);
                }
                outb(base, ' ');
                for (i = index; i <= input_count; i++)
                {
                    outb(base, '\b');
                }

            }
        }
        else
        {
            
    
            next_head = (dcb->ring_head + 1) % sizeof(dcb->ring_buffer);
            if (next_head != dcb->ring_tail && dcb->status != DCB_READING) // Check if buffer is not full
            {
                if (index < input_count)
                {
                    for (i = input_count; i > index; i--)
                    {
                        dcb->ring_buffer[i] = dcb->ring_buffer[i - 1];
                    }
                    dcb->ring_buffer[index] = received_char;
                }
                else
                {
                    dcb->ring_buffer[dcb->ring_head] = received_char;
                }
                dcb->ring_head = next_head;
            }

            // If the device is in DCB_READING state, copy data directly to input buffer
            if (dcb->status == DCB_READING && dcb->input_size > 0)
            {
                if (received_char == '\n' || received_char == '\r')
                {
                    outb(base, '\n');
                    dcb->status = DCB_IDLE;  // Set status to DCB_IDLE
                    dcb->event_flag = EVENT_FLAG_SET;  // Indicate reading completion
                    input_count = 0;
                    index = 0;
                    break;
                }
                if (index < input_count)
                {
                    for (i = input_count; i > index; i--)
                    {
                        dcb->input_buffer[i] = dcb->input_buffer[i - 1];
                        
                    }
                    dcb->input_buffer[index] = received_char;
                    index++;

                    for (i = index - 1; i <= input_count + 1; i++)
                    {
                        outb(base, dcb->input_buffer[i]);
                    }

                    for (i = index; i <= input_count; i++)
                    {
                        outb(base, '\b');
                    }
                }
                else
                {
                    dcb->input_buffer[index++] = received_char;
                    outb(base, received_char);
                }
                input_count++;
                dcb->input_size--;


            }
        }
        }


        //index++;
      
        //INPUT HANDLING LOGIC

       
    sti();
}


/*
* Determine base of IO, same as above
* If logic:
* : dcb->status == DCB_WRITING : Check if dcb is in the writing state
* : && dcb->output_size > 0 : Checks if there is still data left to be sent in the output buffer
*
* : outb(base, *dcb->output_buffer++): Sends one byte of data from output buffer to serial port
*   : *dcb->output_buffer++ : Dereferences pointer to get current byte to be sent, increments to point to
* next character in the buffer
* : dcb->output_size-- : Decrements output_size, which keeps track of how many bytes
* are left in the output buffer to transmit, by one to indicate one byte has been sent
*
* If logic:
* : dcb->output_size == 0 : Checks if all data in the output buffer has been sent
*
* : dcb->status = DCB_IDLE : Set status to DCB_IDLE as port is no longer writing
* : dcb->event_flag = EVENT_FLAG_SET : Set event flag to indicate completion
*
*/
unsigned short base;
unsigned char ier;
void serial_output_interrupt(struct dcb* dcb)
{
    cli();
    base = getDev(dcb->device_id);
    while (dcb->status == DCB_WRITING && dcb->output_size > 0) {
        outb(base, *dcb->output_buffer++);
        dcb->output_size--;
        if (dcb->output_size == 0) {
            dcb->status = DCB_IDLE;          // Set status to DCB_IDLE
            dcb->event_flag = EVENT_FLAG_SET;  // Indicate writing completion

            //disables the transmitter holding register empty interrupt
            ier = inb(base + 1);
            outb(base + 1, ier & ~0x02);
        }
    }
    sti();
}



void serial_interrupt(void)
{
    
    cli();
    for (int dev = 1; dev <= 4; dev++) {
        dcb* dcb = dcb_array[dev-1]; // Retrieve the associated DCB
        if (!dcb || !dcb->open) {
            continue; // If the port is not open, skip
        }

        unsigned short base = getDev(dev); //Get base IO of device
        unsigned char interrupt_id = inb(base + 2); //Get interrupt ID from device r6d_12

        if ((interrupt_id & 0x01) != 0) //Check if interrupt is pending
        {
            continue;
        }

		/*
		*  Now that we have the interrupt_id, we need to determine what type of interrupt it is and handle accordingly
		*  Interrupt type information is stored in bits 1 and 2 r6d_16
		*  unsigned char interrupt_type = interrupt_id & 0x0E;
		*  : interrupt_id & 0x0E : Basic bitwise operator to mask all but bits 1 and 2 of the interrupt_id (0x06 is 00000110 in binary)
		*  Then we use a switch statement on the interrupt type to determine what second-level handler needs called
		*
	    *   Info pulled from r6d_16
	    *   case 0x00: (0000000) Modem Status interrupt: Just read modem status register to clear interrupt, shouldn't happen.
	    *   case 0x02: (00000010) Output Interrupt, call second-level output handler
	    *   case 0x04: (00000100) Input Interrupt, call second-levle input handler
	    *   case 0x06: (00000110) Line Status Interrupt: Just read LSR to clear, shouldn't happen.
	    *
	    */

        unsigned char interrupt_type = interrupt_id & 0x06; //Bitwise to mask non-important bits (keep bits 1 and 2)
        switch (interrupt_type)
        {
        case 0x00:
            inb(base + 6);
            break;
        case 0x02:
            serial_output_interrupt(dcb);
            break;
        case 0x04:
            serial_input_interrupt(dcb);
            break;
        case 0x06:
            inb(base + 5);
            break;
        default:
            break;
        }
       
    }
    outb(0x20, 0x20);
    sti();
    
   


}









