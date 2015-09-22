#ifndef _COMMON_H
#define _COMMON_H

#include <iostream>
#include <SDL.h>

FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}

#endif
