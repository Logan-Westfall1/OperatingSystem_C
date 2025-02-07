#include <stdlib.h>
#include <ctype.h>

int atoi(const char *s)
{
	int res = 0;
	char sign = ' ';

	while (isspace(*s)) {
		s++;
	}

	if (*s == '-' || *s == '+') {
		sign = *s;
		s++;
	}

	while ('0' <= *s && *s <= '9') {
		res = res * 10 + (*s - '0');
		s++;

	}

	if (sign == '-') {
		res = res * -1;
	}

	return res;
}

/*char* itoa(int num) {
    int length = 0;
    int temp = num;
    int is_negative = 0;

    // Handle zero explicitly
    if (num == 0) {
        length = 1;
    } else {
        // Check if the number is negative
        if (num < 0) {
            is_negative = 1;
        }

        // Use a temporary variable to compute the length
        while (temp != 0) {
            // Remove rightmost digit
            temp = temp / 10;
            // Increment digit count by 1
            ++length;
        }
    }

    // Add space for negative sign if necessary
    if (is_negative) {
        ++length;
    }

    // Allocate memory for the string (+1 for the null terminator)
    char* str = (char*)malloc((length + 1) * sizeof(char));
    str[length] = '\0';  // Null terminator

    // Reset temp variable to the original number
    temp = num;
    int index = length - 1;

    // Handle zero case
    if (num == 0) {
        str[0] = '0';
        return str;
    }

    // Convert each digit to a character
    while (temp != 0) {
        int digit = temp % 10;

        // Ensure the digit is positive
        if (digit < 0) {
            digit = -digit;
        }

        str[index--] = '0' + digit;
        temp = temp / 10;
    }

    // Add negative sign if necessary
    if (is_negative) {
        str[0] = '-';
    }

    return str;
}*/
