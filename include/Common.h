#ifndef _COMMON_H
#define _COMMON_H

#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>
#ifdef _APPLE_
#include <OpenGL/glu.h>
#elif (_WIN64 || _WIN32)
#include <gl\GLU.h>
#endif

FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}

#endif
