#ifndef _65FC_MEMORY_H
#define _65FC_MEMORY_H

#include <stdint.h>

// Definition of a memory region.
typedef struct{
	uint16_t size;  // Length of memory region in bytes
	void    *addr;  // Pointer to region in host's memory
	uint8_t  mode;  // Region attribute flags
} MemoryRegion;

enum _memory_map_modes {
    MM_READ    = 0x01,  // Reads come from backing memory
    MM_WRITE   = 0x02,  // Writes go to backing memory
    MM_PULLDN  = 0x04,  // Unmapped reads return 0 (not data bus buffer)
    MM_DYNAMIC = 0x10,  // Backing memory was malloc'd
    MM_OPENBUS = 0x00,  // Not readable, not writeable, not pulled down
};

extern const uint8_t guest_rom[];

extern MemoryRegion memoryMap[];
extern const uint8_t addrSpace;

uint8_t initMemory();
uint8_t readMemory(uint16_t guestAddr);
uint8_t readMemoryZp(uint8_t guestAddr);
uint16_t readMemory16(uint16_t guestAddr);
uint16_t readMemoryZp16(uint8_t guestAddr);
void writeMemory(uint16_t guestAddr, uint8_t byte);

#endif