/**
 * @file string.h
 * @brief Header file for custom string and memory manipulation functions.
 *
 * This file provides declarations for a subset of standard C library functions,
 * including string manipulation, memory operations, and custom printing functions.
 * It also defines macros for colored text output to enhance user interface in the console.
 */

#ifndef MPX_STRING_H
#define MPX_STRING_H

#include <stddef.h>

/**
 * @def BLUE(string)
 * @brief Macro to format a string in blue color.
 * @param string The string to be colored.
 */
#define BLUE(string) "\x1b[34m" string "\x1b[0m"

/**
 * @def RED(string)
 * @brief Macro to format a string in red color.
 * @param string The string to be colored.
 */
#define RED(string) "\x1b[31m" string "\x1b[0m"

/**
 * @def CYAN(string)
 * @brief Macro to format a string in cyan color.
 * @param string The string to be colored.
 */
#define CYAN(string) "\x1b[38;5;123m" string "\x1b[0m"

/**
 * @def YELLOW(string)
 * @brief Macro to format a string in yellow color.
 * @param string The string to be colored.
 */
#define YELLOW(string) "\x1b[33m" string "\x1b[0m"

/**
 * @def GREEN(string)
 * @brief Macro to format a string in green color.
 * @param string The string to be colored.
 */
#define GREEN(string) "\x1b[32m" string "\x1b[0m"

/**
 * @brief Converts an integer to a null-terminated string.
 *
 * This function converts the integer value `num` into a null-terminated string
 * and stores the result in the buffer pointed to by `buffer`.
 *
 * @param num The integer to be converted.
 * @param buffer The buffer where the resulting string will be stored.
 * @return Pointer to the resulting string (`buffer`).
 */
char* itoa(int num, char* buffer);

/**
 * @brief Copies a block of memory from one location to another.
 *
 * This function copies `n` bytes from the memory area pointed to by `src`
 * to the memory area pointed to by `dst`. The memory areas should not overlap.
 *
 * @param dst The destination memory area.
 * @param src The source memory area.
 * @param n The number of bytes to copy.
 * @return A pointer to the destination memory area (`dst`).
 */
void* memcpy(void * restrict dst, const void * restrict src, size_t n);

/**
 * @brief Fills a block of memory with a specified value.
 *
 * This function sets the first `n` bytes of the memory area pointed to by `address`
 * to the specified value `c`.
 *
 * @param address Pointer to the memory area to be filled.
 * @param c The value to be set (converted to an unsigned char).
 * @param n The number of bytes to be set to the value.
 * @return A pointer to the memory area `address`.
 */
void* memset(void *address, int c, size_t n);

/**
 * @brief Compares two null-terminated strings lexicographically.
 *
 * This function compares the string pointed to by `s1` to the string pointed to by `s2`.
 *
 * @param s1 The first string to compare.
 * @param s2 The second string to compare.
 * @return An integer less than, equal to, or greater than zero if `s1` is found,
 *         respectively, to be less than, to match, or be greater than `s2`.
 */
int strcmp(const char *s1, const char *s2);

/**
 * @brief Calculates the length of a null-terminated string.
 *
 * This function computes the length of the string `s` excluding the terminating null byte.
 *
 * @param s A null-terminated string.
 * @return The number of characters in the string `s`.
 */
size_t strlen(const char *s);

/**
 * @brief Converts all uppercase characters in a string to lowercase.
 *
 * This function converts each uppercase letter in the string `s` to its corresponding
 * lowercase letter. Other characters are not affected.
 *
 * @param s The string to be converted to lowercase.
 */
void toLower(char* s);



/**
* @brief Concatenates a variadic number of strings into one new string.
* @param num The number of strings to concatenate
* @param ... The string arguments to be concatenated
* @return The new concatenated string
*/
char* strcat(int num, ...);

/**
@brief Prints a variadic number of specifically formatted help commands to the terminal for easier use within the 'help' functions. Allows for multi-line formatting or single-line.
@param multiLine 0 specifies a single-line format, 1 specifies multi-line
@param num The number of string arguments to be printed
@param ... The string arguments to be printed
*/
void print_help(int multiLine, int num, ...);

/**
@brief Prints a variadic number of specifically formatted help commands to the terminal for easier use within the 'help <function>' functions.
@param num The number of string arguments to be printed
@param ... The string arguments to be printed
*/
void print_detHelp(int num, ...);

/**
 * @brief Splits a string into tokens based on delimiters.
 *
 * This function tokenizes the string `s1` using the delimiters specified in `s2`.
 * It can be called multiple times to obtain all tokens in the string.
 *
 * @param s1 The string to be tokenized, or `NULL` to continue tokenizing the previous string.
 * @param s2 The delimiter string containing characters that separate tokens.
 * @return A pointer to the next token, or `NULL` if there are no more tokens.
 */
char* strtok(char * restrict s1, const char * restrict s2);

/**
 * @brief Outputs a string to the default output device.
 *
 * This function writes the string `string` to the default output device (e.g., console).
 *
 * @param string The string to be printed.
 */
void print(char* string);

/**
 * @brief Outputs a string followed by a newline to the default output device.
 *
 * This function writes the string `string` followed by a newline character to the default output device.
 *
 * @param string The string to be printed.
 */
void println(char* string);

/**
 * @brief Outputs an error message in red color to the default output device.
 *
 * This function writes the string `string` in red color, followed by a newline, to the default output device.
 *
 * @param string The error message to be printed.
 */
void print_e(char* string);

/**
 * @brief Copies up to `n` characters from one string to another.
 *
 * This function copies up to `n` characters from the string `input` to `output`.
 * If `input` is less than `n` characters long, the remainder of `output` is filled with `\0` characters.
 *
 * @param output The destination string buffer.
 * @param input The source string.
 * @param n The maximum number of characters to copy.
 * @return A pointer to the destination string `output`.
 */
char* strncpy(char* output, const char* input, size_t n);


char* itoh(int num);
int htoi(char* hex_str);

#endif /* MPX_STRING_H */
