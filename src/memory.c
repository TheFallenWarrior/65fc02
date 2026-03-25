#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "memory.h"

/* Total size of adressable memory in kilobytes.
   Valid values:
	4:  for 6503, 6505, 6506, 6513, and 6515;
	8:  for 6504, 6507, and 6514;
	64: for 6501, 6502, and 6512. */
const uint8_t addrSpace = 4;

// Simple test harness memory map
MemoryRegion memoryMap[] = {
	{0x0200, NULL, MM_RAM|MM_DYNAMIC},       // 512 bytes of RAM for zeropage and stack
	{0x0600, NULL, MM_OPENBUS},              // Unused
	{0x0800, (void*)guest_rom, MM_READONLY}, // Program ROM
};

// Data bus buffer for open bus emulation
uint8_t dataBusBuf;

// Check if memory map is valid and malloc dynamic regions
uint8_t initMemory(){
	uint16_t memTotal = 0;
	void *tmp;
	uint8_t i, failed = 0, tmp2;

	if(
		addrSpace != 4  &&
		addrSpace != 8  &&
		addrSpace != 64
	) return 1;

	for(i = 0; i < (sizeof(memoryMap)/sizeof(MemoryRegion)); ++i){
		memTotal += memoryMap[i].size;

		tmp2 = memoryMap[i].mode;
		if(tmp2 != MM_ZERO && (tmp2&0x0f) != MM_OPENBUS && (tmp2&0xf0) == MM_DYNAMIC){
			tmp = malloc(memoryMap[i].size);

			if(!tmp){
				failed = 1;
				break;
			} memoryMap[i].addr = tmp;
		}
	}

	if(!failed && memTotal == addrSpace*1024) return 0;

	for(i = 0; i < (sizeof(memoryMap)/sizeof(MemoryRegion)); ++i){
		tmp2 = memoryMap[i].mode;
		if(tmp2 != MM_ZERO && (tmp2&0x0f) != MM_OPENBUS && (tmp2&0xf0) == MM_DYNAMIC){
			if(!memoryMap[i].addr) break;
			free(memoryMap[i].addr);
		}
	}
	return 1;
}

// Get host memory address from guest memory address
void *unmapMemory(uint16_t guestAddr, uint8_t *mode){
	uint16_t memCount = 0;
	uint8_t i;
	
	switch(addrSpace){
		case 4:
		guestAddr = guestAddr & 0x0fff;
		break;

		case 8:
		guestAddr = guestAddr & 0x1fff;
	}

	for(i = 0; i < (sizeof(memoryMap)/sizeof(MemoryRegion)); ++i){
		if((memCount+memoryMap[i].size) > guestAddr){
			*mode = memoryMap[i].mode;
			
			if(!memoryMap[i].addr) return NULL;
			else return ((uint8_t*)memoryMap[i].addr + (guestAddr-memCount));
		}
		memCount += memoryMap[i].size;
	}

	*mode = MM_ZERO;
	return NULL;
}

uint8_t readMemory(uint16_t guestAddr){
	uint8_t mode;
	uint8_t *addr;

	addr = unmapMemory(guestAddr, &mode);

	if((mode&0x0f) == MM_OPENBUS) return dataBusBuf;

	if(mode == MM_ZERO || (mode&0x0f) == MM_WRITONLY || !addr)
		return (dataBusBuf = 0);

	return (dataBusBuf = *addr);
}

void writeMemory(uint16_t guestAddr, uint8_t byte){
	uint8_t mode;
	uint8_t *addr;

	addr = unmapMemory(guestAddr, &mode);
	dataBusBuf = byte;

	if(
		mode == MM_ZERO ||
		(mode&0x0f) == MM_READONLY ||
		(mode&0x0f) == MM_OPENBUS ||
		!addr
	){
		return;
	}

	*addr = byte;
}