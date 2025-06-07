#include "user_tool.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

int strSearch(char* source, char* target)
{
	int len_source = strlen(source);
	int len_target = strlen(target);
	int i, j;
	if (len_source < len_target) {
		return -1;
	}		
	for (i = 0; i <= len_source - len_target; i++) {
		for (j = 0; j < len_target; j++) {
			if (source[i + j] != target[j]) { break;}
		}
		if (j == len_target) { return i;}
	}
	return -1;	
}


uint8_t CharToInt(char* source, uint8_t start, int* number)
{
	uint8_t length;
	uint8_t i;
	int sum = 0;
	int state = 0;
	uint8_t hex = 0;
	length = strlen(source);
	if (start >= length) {
		*number = 0;
		return 0;
	}
	
	if (source[start] == '-') {
		state = -1;
		start++;
	} else {
		state = 1;
		if (start + 1 < length ) {
			if (source[start] == '0' && (source[start + 1] == 'x' ||source[start + 1] == 'X')) {
				start += 2;
				hex = 1;
			}
		}
	}
	for (i = start; i < length; i++) {
		if (source[i] >= '0' && source[i] <= '9') {
			if (hex == 1) {
				sum = sum * 16 + source[i] - '0';
			} else {
				sum = sum * 10 + source[i] - '0';
			}
		} else if (source[i] >= 'a' && source[i] <= 'f') {
			if (hex == 1) {
				sum = sum * 16  + source[i] - 'a' + 10;
			} else {
				*number = 0;
				return 0;
			}
		}else if (source[i] >= 'A' && source[i] <= 'F') {
			if (hex == 1) {
				sum = sum * 16  + source[i] - 'A' + 10;
			} else {
				*number = 0;
				return 0;
			}
		} else if (source[i] == ' ' || source[i] == ',') {
			break;
		}
	}
	*number = state * sum;
	return 1;
}

uint8_t CharToFloat(char* source, uint8_t start, float* number)
{
		uint8_t length;
		uint8_t i;
		float sum = 0;
		int state = 1;
		float smallNumber = 0.1;
		length = strlen(source);
		if (length <= start) {
				*number = 0;
				return 0;
		}
		
		if (source[start] == '-') {
				state = -1;
				start++;
		}
		for (i = start; i < length; i++){
				if (source[i] >= '0' && source[i] <= '9') {
						sum = sum * 10 + source[i] - '0';
				} else if (source[i] == '.') {
						i++;
						break;
				} else if (source[i] == ' ' || source[i] == ',') {
						break;
				}	else {
						*number = 0;
						return 0;
				}					
		}
		for (; i < length; i++) {
				if (source[i] >= '0' && source[i] <= '9') {
						sum += smallNumber * (source[i] - '0');
						smallNumber = smallNumber * 0.1;
				} else if (source[i] == ' ' || source[i] == ',' || source[i] == '\n') {
						break;
				}	else {
						*number = 0;
						return 0;
				}	
		}
		*number = state * sum;
		return 1;
}