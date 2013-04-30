#ifndef _PARSE_H_
#define _PARSE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>




int afreadline(char ** strp, FILE * stream);
int asplit(char *** arrp, const char * str, size_t size, const char * delim);

#endif
