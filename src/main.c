#include <conio.h>
#include <joystick.h>
#include <nes.h>
#include <stdint.h>
#include <stdlib.h>

#include "6500.h"
#include "alu.h"
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

    // Allow up to 4 KiB of malloc'd memory
    _heapadd((void*)0x7000, 0x1000);
    
    if(initMemory()){
        cprintf("Failed to initialize memory.\n");
        while(1);
    }

    // Show free heap memory
    // On the NES, _heapmemavail's output seems to be off by ~42000 bytes
    cprintf("Free memory: %u bytes\r\n\n", _heapmemavail()-42012u);

    reset(); // Reset guest CPU
    while(1){
        // CONIO does not support scrolling; printing past the end of the screen corrupts VRAM
        // Manually check if we reached the end of the screen and clear it
        if(wherey() >= 28){
            clrscr();
            gotoxy(0, 0);
        }

        cputs(fetchDecodeExecute());
        waitForInput(0);
    }

    return 0;
}