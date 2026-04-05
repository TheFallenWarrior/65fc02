#ifndef _65FC_INSTRUCTIONS_H
#define _65FC_INSTRUCTIONS_H

#include <stdint.h>

enum _addressing_modes{
	AM_ACCUMULATOR,
	AM_IMMEDIATE,
	AM_ABSOLUTE,
	AM_ZEROPAGE,
	AM_INDEXED_ZEROPAGE_X,
	AM_INDEXED_ZEROPAGE_Y,
	AM_INDEXED_ABSOLUTE_X,
	AM_INDEXED_ABSOLUTE_Y,
	AM_IMPLIED,
	AM_RELATIVE,
	AM_INDEXED_INDIRECT_X,
	AM_INDIRECT_INDEXED_Y,
	AM_ABSOLUTE_INDIRECT
};

enum _instructions{
	INS_INV, // Invalid		
	INS_ADC,
	INS_AND,
	INS_ASL,
	INS_BCC,
	INS_BCS,
	INS_BEQ,
	INS_BIT,
	INS_BMI,
	INS_BNE,
	INS_BPL,
	INS_BRK,
	INS_BVC,
	INS_BVS,
	INS_CLC,
	INS_CLD,
	INS_CLI,
	INS_CLV,
	INS_CMP,
	INS_CPX,
	INS_CPY,
	INS_DEC,
	INS_DEX,
	INS_DEY,
	INS_EOR,
	INS_INC,
	INS_INX,
	INS_INY,
	INS_JMP,
	INS_JSR,
	INS_LDA,
	INS_LDX,
	INS_LDY,
	INS_LSR,
	INS_NOP,
	INS_ORA,
	INS_PHA,
	INS_PHP,
	INS_PLA,
	INS_PLP,
	INS_ROL,
	INS_ROR,
	INS_RTI,
	INS_RTS,
	INS_SBC,
	INS_SEC,
	INS_SED,
	INS_SEI,
	INS_STA,
	INS_STX,
	INS_STY,
	INS_TAX,
	INS_TAY,
	INS_TSX,
	INS_TXA,
	INS_TXS,
	INS_TYA
};

typedef struct{
	uint8_t instr;
	uint8_t addr_mode;
} Opcode;

typedef struct{
	Opcode operation;
	
	// Instruction parameters
	int8_t param8;
	int16_t param16;

	// Whether the instruction modifies PC directly 
	int8_t pc_mod;
} DecodedInstruction;

extern const uint8_t instruction_length[];
extern const Opcode opcodes[];
extern const char *const mnemonics[];

uint16_t zeropage_addr(uint8_t addr);
uint16_t indexed_absolute_addr(uint16_t addr, uint8_t index);
uint16_t indexed_zeropage_addr(uint8_t addr, uint8_t index);
uint16_t relative_addr(uint16_t addr, int8_t offset);
uint16_t indirect_indexed_addr(uint8_t addr, uint8_t index);
uint16_t indexed_indirect_addr(uint8_t addr, uint8_t index);
uint16_t absolute_indirect_addr(uint16_t addr);

#endif