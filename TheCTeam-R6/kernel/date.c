#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <memory.h>

#include <mpx/serial.h>
#include <processes.h>
#include <sys_req.h>
#include <comHandler.h>
#include <mpx/io.h>
#include <mpx/interrupts.h>
#include "date.h"



/**
 * @brief Convert a hexadecimal value to ASCII.
 *
 * This function converts a single byte of a hexadecimal value
 * to its corresponding ASCII character.
 * 
 * @param hex The hexadecimal value to convert.
 * @return The ASCII representation of the high nibble of the hexadecimal value.
 */

unsigned char htoa(unsigned char hex)
{
	return (hex >> 4) + 0x30;
}

/**
 * @brief Convert ASCII to hexadecimal (BCD).
 *
 * This function takes two ASCII characters representing
 * a BCD-encoded value and converts it into a byte.
 *
 * @param asc The ASCII string representing the BCD value.
 * @return The hexadecimal representation of the BCD value.
 */
unsigned char atoh(char* asc) {
	// Convert the ASCII digits to BCD
	return (((asc[0] - '0') << 4) | (asc[1] - '0'));
	
}


/*
List of registers needed:
Seconds: 0x00
Minutes: 0x02
Hours: 0x04
Day of Week: 0x06
Day of Month: 0x07
Month: 0x08
Year: 0x09
Format: MM-DD-YY HH:MM:SS
*/

/**
 * @brief Retrieves the system date from CMOS registers and prints it.
 *
 * This function reads the system date and time from the CMOS registers,
 * formats it into the "MM-DD-YY HH:MM:SS" format, and outputs it via
 * serial communication.
 */
void get_date(void)
{
	//Read Seconds
	outb(0x70, 0x00);
	unsigned char second = inb(0x71);
	
	//Read Minutes
	outb(0x70, 0x02);
	unsigned char minute = inb(0x71);
	
	//Read Hours
	outb(0x70, 0x04);
	unsigned char hour = inb(0x71);
	
	//Read Day of Month
	outb(0x70, 0x07);
	unsigned char day = inb(0x71);
	
	//Read Month
	outb(0x70, 0x08);
	unsigned char month = inb(0x71);
	
	//Read Year
	outb(0x70, 0x09);
	unsigned char year = inb(0x71);
	
	char* date_message = "MM-DD-YY HH:MM:SS\n";
	sys_req(WRITE, COM1, date_message, strlen(date_message));
	//Print Month
	outb(COM1, htoa(month & 0xF0));
	outb(COM1, htoa((month & 0x0F) << 4));
	outb(COM1, '-');

	//Print Day
	outb(COM1, htoa(day & 0xF0));
	outb(COM1, htoa((day & 0x0F) << 4));
	outb(COM1, '-');


	//Print Year
	outb(COM1, htoa(year & 0xF0));
	outb(COM1, htoa((year & 0x0F) << 4));
	outb(COM1, ' ');


	//Print Hour
	outb(COM1, htoa(hour & 0xF0));
	outb(COM1, htoa((hour & 0x0F) << 4));
	outb(COM1, ':');


	//Print Minute
	outb(COM1, htoa(minute & 0xF0));
	outb(COM1, htoa((minute & 0x0F) << 4));
	outb(COM1, ':');

	//Print Second
	outb(COM1, htoa(second & 0xF0));
	outb(COM1, htoa((second & 0x0F) << 4));
	outb(COM1, '\n');
}

char* get_time(void) //CREATED FOR R4 ALARMS
{
	//Read Seconds
	outb(0x70, 0x00);
	unsigned char secondA = inb(0x71);

	//Read Minutes
	outb(0x70, 0x02);
	unsigned char minuteA = inb(0x71);

	//Read Hours
	outb(0x70, 0x04);
	unsigned char hourA = inb(0x71);

	unsigned char second1 = htoa(secondA & 0xF0);
	unsigned char second2 = htoa((secondA & 0x0F) << 4);
	unsigned char minute1 = htoa(minuteA & 0xF0);
	unsigned char minute2 = htoa((minuteA & 0x0F) << 4);
	unsigned char hour1 = htoa(hourA & 0xF0);
	unsigned char hour2 = htoa((hourA & 0x0F) << 4);

	char* time = (char*)sys_alloc_mem(7 * sizeof(char));
	time[0] = hour1;
	time[1] = hour2;
	time[2] = minute1;
	time[3] = minute2;
	time[4] = second1;
	time[5] = second2;
	time[6] = '\0';

	return time;
	
}

/**
 * @brief Validates the date and time components for correctness.
 *
 * This function checks the date and time components to ensure they
 * are valid, including month, day, year, hour, minute, and second.
 *
 * @param tokDate Array of string tokens representing the date and time.
 * @return 1 if the date is valid, -1 if invalid.
 */
int validate_date(char* tokDate[6])
{
	char* invalidInput = RED("Error: Invalid date input entered. Usage: 'date set MM-DD-YY HH:MM:SS' use 'help date' for more info\n");
	for (int i = 0; i < 6; i++)
	{
		if (tokDate[i] == 0)
		{
			sys_req(WRITE, COM1, invalidInput, strlen(invalidInput));
			return -1;
		}
	}
	unsigned char month = (tokDate[0][0] - '0') * 10 + (tokDate[0][1] - '0');
	unsigned char day = (tokDate[1][0] - '0') * 10 + (tokDate[1][1] - '0');
	unsigned char year = (tokDate[2][0] - '0') * 10 + (tokDate[2][1] - '0');
	unsigned char hour = (tokDate[3][0] - '0') * 10 + (tokDate[3][1] - '0');
	unsigned char minute = (tokDate[4][0] - '0') * 10 + (tokDate[4][1] - '0');
	unsigned char second = (tokDate[5][0] - '0') * 10 + (tokDate[5][1] - '0');
	
	int daysInMonth;
	/*
	* Need to check month is 1-12
	* Need to check day fits within month boundaries:
	* 30 days: april, june, september, november
	* 31 days: january, march, may, july, august, october, december
	* 28 days: february on years not divisible by 4
	* 29 days: february on years divisible by 4
	* Year is 00-99
	* Hour is 00-23
	* Minute is 00-59
	* Second is 00-59
	*/

	

	//Month validation
	if (month > 12 || month < 1)
	{
		print_e("Date: invalid month entered");
		return -1;
	}

	//Day validation
	switch (month)
	{
	case 1: case 3: case 5: case 7: case 8: case 10: case 12:
		daysInMonth = 31;
		break;
	case 4: case 6: case 9: case 11:
		daysInMonth = 30;
		break;
	case 2:
		daysInMonth = ((year % 4 == 0 && (year + 2000) % 100 != 0) || ((year + 2000) % 400 == 0)) ? 29 : 28;
		break;
	default:
		return -1;
	}

	if (day < 0 || day > daysInMonth)
	{
		print_e("Date: Invalid day entered");
		return -1;
	}

	//Year validation
	if (year < 0 || year > 99)
	{
		print_e("Date: Invalid year entered");
		return -1;
	}

	//Hour Validation
	if (hour < 0 || hour > 23)
	{
		print_e("Date: Invalid hour entered");
		return -1;
	}

	
	//Minute validation
	if (minute < 0 || minute > 59)
	{
		print_e("Date: Invalid minute entered");
		return -1;
	}

	//Seond validation
	if (second < 0 || second > 59)
	{
		print_e("Date: Invalid second entered");
		return -1;
	}
	
	else
	{
		return 1;
	}
}


/**
 * @brief Sets the system date using user input.
 *
 * This function takes the user input date and time, validates it,
 * and then sets it in the CMOS registers.
 *
 * @param buf The buffer containing the MM-DD-YY part of the date.
 * @param buf2 The buffer containing the HH:MM:SS part of the time.
 * @return 1 if the date was successfully set, -1 otherwise.
 */
int set_date(char* buf, char* buf2)
{
	//MM-DD-YY;HH:MM:SS <--- User input format (Can actually use any delimiters defined above, doesn't matter)
	int toks = 0;
	char* tokDate[6] = { 0 };
	char* token = strtok(buf, DELIMITERS);
	while (token != NULL && toks < 3)
	{
		tokDate[toks++] = token;
		token = strtok(NULL, DELIMITERS);
	}
	token = strtok(buf2, DELIMITERS);
	while (token != NULL && toks < 6)
	{
		tokDate[toks++] = token;
		token = strtok(NULL, DELIMITERS);
	}
	
	if (validate_date(tokDate) != -1)
	{
		cli();

		//Set Month
		outb(0x70, 0x08);
		outb(0x71, atoh(tokDate[0]));
	

		//Set Day
		outb(0x70, 0x07);
		outb(0x71, atoh(tokDate[1]));

		//Set Year
		outb(0x70, 0x09);
		outb(0x71, atoh(tokDate[2]));
	

	
		//Set Hour
		outb(0x70, 0x04);
		outb(0x71, atoh(tokDate[3]));
	

		//Set Minute
		outb(0x70, 0x02);
		outb(0x71, atoh((tokDate[4])));
	
		//Set Second
		outb(0x70, 0x00);
		outb(0x71, atoh(tokDate[5]));
	
		sti();
		println(GREEN("Date set successfully"));
		return 1;
	}
	
	return -1;
}

