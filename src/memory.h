#ifndef _65FC_MEMORY_H
#define _65FC_MEMORY_H

#include <stdint.h>

// Definition of a memory region.
typedef struct{
	uint16_t size;  // Length of memory region in bytes
	void    *addr;  // Pointer to region in host's memory
	uint8_t  mode;  // Region attribute flags
} MemoryRegion;

// Flags for region attributes.
enum _memory_map_modes{
	MM_RAM      = 0x00, // Readable and writeable memory
	MM_ZERO     = 0x01, // Returns 0 when read, implies not dynamic
	MM_READONLY = 0x02, // Read only memory
	MM_WRITONLY = 0x03, // Write-only, returns 0 when read
	MM_OPENBUS  = 0x04, // Open bus
	MM_DYNAMIC  = 0x10, // Malloc'd memory region
};

extern const uint8_t guest_rom[];

extern MemoryRegion memoryMap[];
extern const uint8_t addrSpace;

uint8_t initMemory();
uint8_t readMemory(uint16_t guestAddr);
void writeMemory(uint16_t guestAddr, uint8_t byte);

#endif