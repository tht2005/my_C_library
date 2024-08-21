#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdlib.h>

void* ptr_handler(void* ptr, const char *msg) {
	if(ptr == NULL) {
		perror(msg);
		exit(1);
	}
	return ptr;
}

#endif
