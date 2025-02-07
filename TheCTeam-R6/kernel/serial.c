/**
 * @file serial_comm.c
 * @brief Serial communication handling for input and output operations via COM ports.
 * @authors Logan Westfall, Brendan Bolduc, Jacob Bock, Evan Anderson
 * This file contains functions to initialize serial communication, send data through
 * serial ports, and poll input while handling special key inputs such as backspace,
 * delete, and arrow keys.
 */

#include <mpx/io.h>
#include <comHandler.h>
#include <mpx/serial.h>
#include <sys_req.h>
#include <string.h>

#define LSR_DATA_READY 0x01 //Define bit to verify LSR has data ready to write to buffer

//Ascii values for special keys that need handled
#define KEY_BACKSPACE  0x08
#define MAC_DELETE 0x7F
#define WIN_DELETE 0x2E
#define KEY_CARRIAGE_RETURN 0x0D // \r
#define KEY_NEWLINE 0x0A // \n


size_t strlen(const char *str);

enum uart_registers {
	RBR = 0,	// Receive Buffer
	THR = 0,	// Transmitter Holding
	DLL = 0,	// Divisor Latch LSB
	IER = 1,	// Interrupt Enable
	DLM = 1,	// Divisor Latch MSB
	IIR = 2,	// Interrupt Identification
	FCR = 2,	// FIFO Control
	LCR = 3,	// Line Control
	MCR = 4,	// Modem Control
	LSR = 5,	// Line Status
	MSR = 6,	// Modem Status
	SCR = 7,	// Scratch
};

static int initialized[4] = { 0 };

static int serial_devno(device dev)
{
	switch (dev) {
	case COM1: return 0;
	case COM2: return 1;
	case COM3: return 2;
	case COM4: return 3;
	}
	return -1;
}

/**
 * @brief Initializes the serial communication for the given device.
 *
 * This function configures the serial port to work with 9600 baud rate, 8 bits data, 
 * no parity, and one stop bit. It also enables FIFO, sets the RTS/DSR, and resets the port.
 * 
 * @param dev The COM port to be initialized (e.g., COM1, COM2).
 * @return 0 on success, -1 if the device is not recognized.
 */
int serial_init(device dev)
{
	int dno = serial_devno(dev);
	if (dno == -1) {
		return -1;
	}
	outb(dev + IER, 0x00);	//disable interrupts
	outb(dev + LCR, 0x80);	//set line control register
	outb(dev + DLL, 115200 / 9600);	//set bsd least sig bit
	outb(dev + DLM, 0x00);	//brd most significant bit
	outb(dev + LCR, 0x03);	//lock divisor; 8bits, no parity, one stop
	outb(dev + FCR, 0xC7);	//enable fifo, clear, 14byte threshold
	outb(dev + MCR, 0x0B);	//enable interrupts, rts/dsr set
	(void)inb(dev);		//read bit to reset port
	initialized[dno] = 1;
	return 0;
}

/**
 * @brief Outputs data through the serial port.
 *
 * This function writes the data from the buffer to the specified serial port.
 * It loops over the buffer and sends each character.
 *
 * @param dev The COM port to send data to (e.g., COM1, COM2).
 * @param buffer The data buffer containing characters to be sent.
 * @param len The length of the buffer.
 * @return The number of bytes sent, or -1 if the device is not recognized or not initialized.
 */
int serial_out(device dev, const char* buffer, size_t len)
{
	int dno = serial_devno(dev);
	if (dno == -1 || initialized[dno] == 0) {
		return -1;
	}
	for (size_t i = 0; i < len; i++) {
		outb(dev, buffer[i]);
	}
	return (int)len;
}

/**
 * @brief Polls for input from the serial port.
 *
 * This function continuously reads input from the specified serial port and handles special
 * key inputs such as backspace, delete, and arrow keys. It modifies the input buffer based on 
 * the user's input. The function also handles inserting and deleting characters, as well as handling
 * the up and down arrow keys for history (currently commented).
 * 
 * @param dev The COM port to poll input from (e.g., COM1, COM2).
 * @param buffer The buffer to store the received input.
 * @param len The maximum length of the input buffer.
 * @return The number of characters read, or -1 if an error occurs.
 */
int serial_poll(device dev, char* buffer, size_t len)
{


	//com1 + LSR bit manipulation to see if lsb of lsr is 1
	size_t index = 0;

	while (index < len) {

		if (inb(dev + LSR) & LSR_DATA_READY)
		{
			char c = inb(dev);
			int WIN_DELETE_SEQ = 0;
			char check;




 			// arrow key code that does work but appears not to
			if (c == 27)  //ASCII for ESC
			{
				while(inb(dev + LSR) & 0x01){
				check = inb(dev);
				if (check == 91) //ASCII for '['
				{
					check = inb(dev);
					if (check == 68) //ASCII for 'D' | left arrow
					{
						if (index > 0)
						{
						index--;
						outb(dev, '\b');
						}
					}else if(check == 67) //ASCII for 'C' | right arrow
					{
						if (index < len)
						{
						outb(dev, buffer[index]); //reprints characters one at a time from buffer as cursor moves right
						index++;
						}
					}else if (check == 65)
					{
					//up arrow code
					/*
						if (histry_count > 0) 
						{
							current_history_index = (current_history_inndex - 1 + history_count ) % history count;
							strcpy(buffer, history[current_history_index]);
							index = strlen(buffer);
							serial_out(dev, buffer, index);
						}
					*/
					}else if( check == 66)
					{
					// down arrow code
					/*
						if(history_count > 0 && current_history_index >= 0)
							{
								current_history_index = (current_histyory_indux + 1) % history_count;
								if (current_history_index == history_count) current_history_index = -1;
								if (current_history_index != -1) 
								{
									strcpy(buffer, history[current_history_index]);
									index = strlen(buffer);
									serial_out(dev,buffer,index);
								} 
							}
					*/
					
						
					}else if (check == '3')
                    {
						while(inb(dev + LSR) & 0x01){
                        	check = inb(dev);
                        	if (check == '~')
                        	{
                        	    WIN_DELETE_SEQ = 1;
                        	}
						}
                    }

				}}
			}



			if (c == WIN_DELETE || WIN_DELETE_SEQ == 1)
			{
				//checks we are not at the end of the line
				if (index < strlen(buffer))
				{
					
                    for (size_t i = index ; i <= strlen(buffer)-1; i++) {
                        buffer[i] = buffer[i + 1];
                    }


                    //moves the cursor back and reprint the shifted characters
                    for (size_t i = index; i <= strlen(buffer)-1; i++) {
                        outb(dev, buffer[i]);
                    }

                    //prints space to overwrite the last character

					outb(dev, ' ');

                    //moves the cursor back to its original position
                    for (size_t i = index; i <= strlen(buffer); i++) {
                        outb(dev, '\b');
                    }
				}
				//resets delete sequence
				WIN_DELETE_SEQ = 0;
			}		



			if(c == KEY_BACKSPACE || c == MAC_DELETE)
			{
				if (index > 0) {
                    //shifts the buffer left by one position
                    index--;
                    for (size_t i = index; i < strlen(buffer); i++) {
                        buffer[i] = buffer[i + 1];
                    }

                    size_t current_len = strlen(buffer);

                    //moves the cursor back and reprint the shifted characters
                    outb(dev, '\b');
                    for (size_t i = index; i < current_len; i++) {
                        outb(dev, buffer[i]);
                    }

                    //prints space to overwrite the last character
                    outb(dev, ' ');

                    //moves the cursor back to its original position
                    for (size_t i = index; i <= current_len; i++) {
                        outb(dev, '\b');
                    }
                }
			}



			int clear_buf = 0;
			if (c == KEY_CARRIAGE_RETURN || c == KEY_NEWLINE)
			{

				if(index  < strlen(buffer))
				{
					for(size_t i = index; i <= strlen(buffer); i++)
					{
						index++;
					}

				}
				//Code to handle these two keys, can separate these as well
				buffer[index] = '\0';
				outb(dev, '\n');
				clear_buf = 1;
				break;
			}


			if(clear_buf == 1)
			{
				for(size_t i = 0; i <= strlen(buffer); i++)
				{
					buffer[i] = '0';
				}
				index = 0;
			}

			if (((c >= 32 && c <= 126) || c == ' '))
			{
				if(index > 0 &&  index < strlen(buffer))
				{
					for(size_t i = strlen(buffer); i > index; i--)
                    {
						buffer[i] = buffer[i - 1];
                    }

					buffer[index] = c;
					index++;
					
        			for (size_t i = index-1; i <= strlen(buffer)+1; i++)
        			{
            			outb(dev, buffer[i]);
        			}

					for (size_t i = index+1; i <= strlen(buffer); i++)
        			{
            			outb(dev, '\b');
        			}
				} else { 
					buffer[index++] = c;
					outb(dev, c);
				}
			}
			
		}

	}

	//Initialize the command history 
	//create a list to store command history
	//create a var for last 5 commands, index for current command 
	//and current command being typed

	//add to the history func
		//if the length of the history is >= size then
			//remove oldest command

		//add new command to history
		//reset index to end
	//function to handle input
		//while recieving input read that chars
		//if user returns - save to history
		//else if char is up arrow then navigate up in history
			//move index from history
		//else if down arrow 
			//navigate history
		//if the index is at the end clear the line
		//display the command in history
		//else add the characters to end of the command

	//func to dosplay current command
		//clear the line
		//print the command in history at the right index
		//move cursor to end of the line
		
	
	// THIS MUST BE CHANGED TO RETURN THE CORRECT VALUE
	return (int)index;
}

