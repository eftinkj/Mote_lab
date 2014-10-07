
#include <stdio.h>
#include <rt_misc.h>

#pragma import(__use_no_semihosting_swi)
#define _LLIO_STDIN ((int) stdin)
#define _LLIO_STDOUT ((int) stdout)
#define _LLIO_STDERR ((int) stderr)
#define __write _write
#define __read _read

extern size_t _write(int handle, const unsigned char * buffer, size_t size);
extern size_t _read(int handle, unsigned char * buffer, size_t size);

struct __FILE { int handle; /* Add whatever you need here */ };


int fputc(int c, FILE *f) {

	__write(_LLIO_STDOUT, (unsigned char *)&c, 1);
		return (c);
}

int fgetc(FILE *f) {
	int c;
	int nbchar;
  nbchar = __read(_LLIO_STDIN, (unsigned char *)&c, 1);
	return ((nbchar==0)?-1:c);
}


void _ttywrch(int ch) {
  volatile unsigned int i;
}


void _sys_exit(int return_code) {
  while (1);    /* endless loop */
}

