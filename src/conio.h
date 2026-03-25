#ifndef _65FC_CONIO_H
#define _65FC_CONIO_H

#ifdef __NES__
	#include <conio.h>
	#include <joystick.h>
#else
	#include <stdio.h>

	#define clrscr() (void)0
	#define cprintf(args...) printf(args)
#endif

#endif