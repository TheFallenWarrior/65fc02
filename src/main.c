#include <nes.h>
#include <stdint.h>
#include <stdlib.h>

#include "6500.h"
#include "alu.h"
#include "conio.h"
#include "instructions.h"
#include "memory.h"

// Waits for input specified by a mask and then returns it
uint8_t waitForInput(uint8_t mask){
	if(!mask) mask = 0xff;
	while(joy_read(JOY_1));
	while(!(mask&joy_read(JOY_1)));
	return joy_read(JOY_1);
}

int main(){
    joy_install(joy_static_stddrv);
    clrscr();

    // This fixes a problem with the cc65 runtime that causes malloc to fail
    _heapadd((void*)0x6800, 0x800);
    
    if(initMemory()){
        cprintf("Failed to initialize memory.\n");
        while(1);
    }

    // I don't particularly trust _heapmemavail's output but display it anyway
    cprintf("Free memory: %u bytes\r\n\n", _heapmemavail());

    reset(); // Reset guest CPU
    while(1){
        // CONIO does not support scrolling; printing past the end of the screen corrupts VRAM
        // Manually check if we reached the end of the screen and clear it
        if(wherey() >= 28){
            clrscr();
            gotoxy(0, 0);
        }

        fetchDecodeExecute(1);
        waitForInput(0);
    }

    return 0;
}