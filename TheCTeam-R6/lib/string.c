#include <string.h>
#include <sys_req.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

#include <mpx/serial.h>
#include <processes.h>
#include <sys_req.h>
#include <comHandler.h>
#include <mpx/io.h>
#include <pcb.h>
#include <memory.h>
#include <mem_lib.h>
#include <mpx/interrupts.h>

#define BLUE(string) "\x1b[34m" string "\x1b[0m"
#define RED(string) "\x1b[31m" string "\x1b[0m"
#define CYAN(string) "\x1b[38;5;123m" string "\x1b[0m"
#define YELLOW(string) "\x1b[33m" string "\x1b[0m"
#define GREEN(string) "\x1b[32m" string "\x1b[0m"

char hex_num[] = "0123456789ABCDEF";


char* itoa(int num, char* buffer) {
    
    int i = 0;
    int isNegative = 0;

    if(num == 0) { //Handle 0
        buffer[i++] = '0';
        buffer[i] = '\0';
        return buffer;
    }

    if(num < 0) { //Handle negative numbers
        isNegative = 1;
        num = -num;
    }

    
    while(num != 0) {
        buffer[i++] = (num % 10) + '0'; 
        num /= 10;                     
    }

    if(isNegative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';

    // Reverse the string 
    int start = 0;
    int end = i - 1;
    char temp;
    while(start < end) {
        temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }

    return buffer;
}


   


/* memcpy() and memset() are in core.c */

int strcmp(const char *s1, const char *s2)
{

	// Remarks:
	// 1) If we made it to the end of both strings (i. e. our pointer points to a
	//    '\0' character), the function will return 0
	// 2) If we didn't make it to the end of both strings, the function will
	//    return the difference of the characters at the first index of
	//    indifference.
	while ((*s1) && (*s1 == *s2)) {
		++s1;
		++s2;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

size_t strlen(const char *s)
{
	size_t len = 0;
	while (*s++) {
		len++;
	}
	return len;
}


//NEW
void toLower(char *s)
{
;	while (*s)
	{
		if (*s >= 'A' && *s <= 'Z')
		{
			*s = *s + ('a' - 'A');
		}
		s++;
	}
}

char* strtok(char* restrict s1, const char* restrict s2)
{
    static char* tok_tmp = NULL;

    // If a new string is provided, use it; otherwise, continue with the old string
    if (s1 != NULL) {
        tok_tmp = s1;
    }
    else {
        if (tok_tmp == NULL) {
            return NULL;
        }
        s1 = tok_tmp;
    }

    // Skip leading delimiters
    while (*s1) {
        const char* p = s2;
        while (*p) {
            if (*s1 == *p) {
                ++s1;
                break;
            }
            ++p;
        }
        if (*p == '\0') {  // No delimiter found, break out
            break;
        }
    }

    // If we reached the end of the string, return NULL
    if (*s1 == '\0') {
        tok_tmp = NULL;
        return NULL;
    }

    // Mark the beginning of the token
    tok_tmp = s1;

    // Skip non-delimiter characters
    while (*tok_tmp) {
        const char* p = s2;
        while (*p) {
            if (*tok_tmp == *p) {
                *tok_tmp++ = '\0';
                return s1;
            }
            ++p;
        }
        ++tok_tmp;
    }

    // We've reached the end of the string, so set `tok_tmp` to NULL
    tok_tmp = NULL;
    return s1;
}


void print(char* string){
    sys_req(WRITE, COM1, string, strlen(string));
}

void println(char* string){
    sys_req(WRITE, COM1, string, strlen(string));
    sys_req(WRITE, COM1, "\n", 1);
}

void print_e(char* string){
    sys_req(WRITE, COM1, "\x1b[31m", strlen("\x1b[31m"));
    sys_req(WRITE, COM1, string, strlen(string));
    sys_req(WRITE, COM1, "\n", 1);
    sys_req(WRITE, COM1, "\x1b[0m", strlen("\x1b[0m"));
}


char* strcat(int num, ...)
{
    cli();
    va_list args;
    int total_length = 0;
    
    va_start(args, num);
    for (int i = 0; i < num; i++)
    {
        const char* s = va_arg(args, const char*);
        total_length += strlen(s);
    }
    va_end(args);
    
    
    char* dest = (char*)sys_alloc_mem((total_length + 1) * sizeof(char));
    
    va_start(args, num);
    int pos = 0;
    for (int i = 0; i < num; i++)
    {
        const char* s = va_arg(args, const char*);
        while (*s != '\0')
        {
            dest[pos++] = *s++;
        }
    }
    va_end(args);
    sti();
    dest[pos] = '\0';

    return dest;

}


void print_help(int multiLine, int num, ...)
{
    char* cat_string;
    cli();
    va_list args;
    va_start(args, num);

    if (!multiLine)
    {
        cat_string = strcat(num + 2, "\x1b[33m", va_arg(args, const char*), "\x1b[0m: ", va_arg(args, const char*));
        println(cat_string);
        sys_free_mem(cat_string);
    }
    if (multiLine)
    {
        cat_string = strcat(3, "\x1b[33m", va_arg(args, const char*), "\x1b[0m: ");
        println(cat_string);
        sys_free_mem(cat_string);
        
        for (int i = 1; i < num; i++)
        {
            cat_string = strcat(2, "\t", va_arg(args, const char*));
            println(cat_string);
            sys_free_mem(cat_string);
        }
    }
    sti();
}

void print_detHelp(int num, ...)
{
    char* cat_string;
    cli();
    va_list args;
    va_start(args, num);

    cat_string = strcat(4, "\x1b[33m", va_arg(args, const char*), "\x1b[0m:\n", va_arg(args, const char*));
    println(cat_string);
    sys_free_mem(cat_string);
    
    for (int i = 2; i < num; i++)
    {
        cat_string = strcat(2, "\t", va_arg(args, const char*));
        println(cat_string);
        sys_free_mem(cat_string);
    }
    sti();
    println("");
}

/*void print_help(char* s1, char* s2)
{
    println(strcat(4, "\x1b[33m", s1, "\x1b[0m: ", s2));
}
*/
char* strncpy(char* output, const char* input, size_t n) {
    size_t i;

    //copies up to n characters from input to output
    for (i = 0; i < n && input[i] != '\0'; i++){
        output[i] = input[i];
    }

    //pads the rest of dest with null characters if src is shorter than n
    while(i < n)
    {
        output[i] = '\0';
        i++;
    }

    return output;
}

char* itoh(int num){
    char* output = (char*) sys_alloc_mem(9 * sizeof(char));
    //quick error check
    if (output == NULL) {
        print_e("Error: Failed to allocate memory in itoh");
        return NULL;
    }

    //0 edge case
    if (num == 0) {
        output[0] = '0';
        output[1] = '\0';
        return output;
    }

    int temp = num;
    int r;
    //8 is max size for int with 9 null terminator
    char flip[9];
    int track = 0;
    while(temp > 0 && track < 8){
        r = temp % 16;
        flip[track++] = hex_num[r];
        track++;
        temp = temp / 16;
    }


    int o_track = 0;
    for(int i = track - 1; i >= 0; i--){
        output[o_track] = flip[i];
        o_track++;
    }
    output[o_track] = '\0';

    return output;
}


int htoi(char* hex_str) {
    int result = 0;
    int i = 0;

    // Skip "0x" or "0X" prefix if present
    if (hex_str[0] == '0' && (hex_str[1] == 'x' || hex_str[1] == 'X')) {
        i = 2;
    }

    // Iterate through each character of the hex string
    while (hex_str[i] != '\0') {
        result = result * 16;  // Shift result left by one hex place (4 bits)

        // Convert the current hex digit to its integer value
        if (hex_str[i] >= '0' && hex_str[i] <= '9') {
            result += hex_str[i] - '0';
        }
        else if (hex_str[i] >= 'A' && hex_str[i] <= 'F') {
            result += hex_str[i] - 'A' + 10;
        }
        else if (hex_str[i] >= 'a' && hex_str[i] <= 'f') {
            result += hex_str[i] - 'a' + 10;
        }
        else {
            // Handle invalid characters
            return -1;  // Return -1 for an invalid character
        }

        i++;
    }

    return result;
}



