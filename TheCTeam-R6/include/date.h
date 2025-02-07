
#define DELIMITERS "-;:/"
/**
 @file date.h
 @brief Kernel functions for date commands
*/

/**
 * @brief Retrieves the system date from CMOS registers and prints it.
 *
 * This function reads the system date and time from the CMOS registers,
 * formats it into the "MM-DD-YY HH:MM:SS" format, and outputs it via
 * serial communication.
 */
void get_date(void);

char* get_time(void);

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
int set_date(char* buf, char* buf2);

/**
 * @brief Validates the date and time components for correctness.
 *
 * This function checks the date and time components to ensure they
 * are valid, including month, day, year, hour, minute, and second.
 *
 * @param tokDate Array of string tokens representing the date and time.
 * @return 1 if the date is valid, -1 if invalid.
 */
int validate_date(char* tokDate[6]);

/**
 * @brief Convert ASCII to hexadecimal (BCD).
 *
 * This function takes two ASCII characters representing
 * a BCD-encoded value and converts it into a byte.
 *
 * @param asc The ASCII string representing the BCD value.
 * @return The hexadecimal representation of the BCD value.
 */
unsigned char atoh(char* asc);

/**
 * @brief Convert a hexadecimal value to ASCII.
 *
 * This function converts a single byte of a hexadecimal value
 * to its corresponding ASCII character.
 * 
 * @param hex The hexadecimal value to convert.
 * @return The ASCII representation of the high nibble of the hexadecimal value.
 */

unsigned char htoa(unsigned char hex);
