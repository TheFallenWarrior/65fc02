#ifndef _65FC_6500_H
#define _65FC_6500_H

#include <stdint.h>

#include "instructions.h"

// Flags for the processor status registrer
#define PFLAG_N 0x80 // Negative
#define PFLAG_V 0x40 // Overflow
#define PFLAG_B 0x10 // Break
#define PFLAG_D 0x08 // Decimal mode enable
#define PFLAG_I 0x04 // Interrupt disable
#define PFLAG_Z 0x02 // Zero
#define PFLAG_C 0x01 // Carry

#define VEC_NMI 0xfffa // Non-maskable interrupt vector
#define VEC_RST 0xfffc // Reset (entry point) vector
#define VEC_IRQ 0xfffe // Interrupt request vector

// Get effective stack pointer
#define STACK_PTR() (0x0100 | mcs6500.s)

// Internal state of a 65xx processor
typedef struct{
	uint8_t a, x, y; // Accumulator and index registers
	uint8_t p;       // Processor status register
	uint8_t ir;      // Instruction register
	uint8_t s;       // Stack pointer
	uint16_t pc;     // Program counter
} Processor;

extern Processor mcs6500;

void pushToStack(uint8_t byte);
uint8_t pullFromStack();
void reset();
uint16_t decodeInstruction(DecodedInstruction *ins);
void executeInstruction(DecodedInstruction *ins);

void fetchDecodeExecute(uint8_t debug_enable);

#endif