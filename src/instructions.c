#include <stdint.h>
#include <string.h>

#include "6500.h"
#include "conio.h"
#include "instructions.h"
#include "memory.h"

const char *const mnemonics[] = {
	"INV",
	"ADC",
	"AND",
	"ASL",
	"BCC",
	"BCS",
	"BEQ",
	"BIT",
	"BMI",
	"BNE",
	"BPL",
	"BRK",
	"BVC",
	"BVS",
	"CLC",
	"CLD",
	"CLI",
	"CLV",
	"CMP",
	"CPX",
	"CPY",
	"DEC",
	"DEX",
	"DEY",
	"EOR",
	"INC",
	"INX",
	"INY",
	"JMP",
	"JSR",
	"LDA",
	"LDX",
	"LDY",
	"LSR",
	"NOP",
	"ORA",
	"PHA",
	"PHP",
	"PLA",
	"PLP",
	"ROL",
	"ROR",
	"RTI",
	"RTS",
	"SBC",
	"SEC",
	"SED",
	"SEI",
	"STA",
	"STX",
	"STY",
	"TAX",
	"TAY",
	"TSX",
	"TXA",
	"TXS",
	"TYA"
};

// Instruction length in bytes per addressing mode
const uint8_t instruction_length[] = {
	1,
	2,
	3,
	2,
	2,
	2,
	3,
	3,
	1,
	2,
	2,
	2,
	3
};

const Opcode opcodes[] = {
    // 0x00 - 0x0F
    {INS_BRK, AM_IMPLIED},            // 0x00
    {INS_ORA, AM_INDEXED_INDIRECT_X}, // 0x01
    {INS_INV, AM_IMPLIED},            // 0x02
    {INS_INV, AM_IMPLIED},            // 0x03
    {INS_INV, AM_IMPLIED},            // 0x04
    {INS_ORA, AM_ZEROPAGE},           // 0x05
    {INS_ASL, AM_ZEROPAGE},           // 0x06
    {INS_INV, AM_IMPLIED},            // 0x07
    {INS_PHP, AM_IMPLIED},            // 0x08
    {INS_ORA, AM_IMMEDIATE},          // 0x09
    {INS_ASL, AM_ACCUMULATOR},        // 0x0A
    {INS_INV, AM_IMPLIED},            // 0x0B
    {INS_INV, AM_IMPLIED},            // 0x0C
    {INS_ORA, AM_ABSOLUTE},           // 0x0D
    {INS_ASL, AM_ABSOLUTE},           // 0x0E
    {INS_INV, AM_IMPLIED},            // 0x0F

    // 0x10 - 0x1F
    {INS_BPL, AM_RELATIVE},           // 0x10
    {INS_ORA, AM_INDIRECT_INDEXED_Y}, // 0x11
    {INS_INV, AM_IMPLIED},            // 0x12
    {INS_INV, AM_IMPLIED},            // 0x13
    {INS_INV, AM_IMPLIED},            // 0x14
    {INS_ORA, AM_INDEXED_ZEROPAGE_X}, // 0x15
    {INS_ASL, AM_INDEXED_ZEROPAGE_X}, // 0x16
    {INS_INV, AM_IMPLIED},            // 0x17
    {INS_CLC, AM_IMPLIED},            // 0x18
    {INS_ORA, AM_INDEXED_ABSOLUTE_Y}, // 0x19
    {INS_INV, AM_IMPLIED},            // 0x1A
    {INS_INV, AM_IMPLIED},            // 0x1B
    {INS_INV, AM_IMPLIED},            // 0x1C
    {INS_ORA, AM_INDEXED_ABSOLUTE_X}, // 0x1D
    {INS_ASL, AM_INDEXED_ABSOLUTE_X}, // 0x1E
    {INS_INV, AM_IMPLIED},            // 0x1F

    // 0x20 - 0x2F
    {INS_JSR, AM_ABSOLUTE},           // 0x20
    {INS_AND, AM_INDEXED_INDIRECT_X}, // 0x21
    {INS_INV, AM_IMPLIED},            // 0x22
    {INS_INV, AM_IMPLIED},            // 0x23
    {INS_BIT, AM_ZEROPAGE},           // 0x24
    {INS_AND, AM_ZEROPAGE},           // 0x25
    {INS_ROL, AM_ZEROPAGE},           // 0x26
    {INS_INV, AM_IMPLIED},            // 0x27
    {INS_PLP, AM_IMPLIED},            // 0x28
    {INS_AND, AM_IMMEDIATE},          // 0x29
    {INS_ROL, AM_ACCUMULATOR},        // 0x2A
    {INS_INV, AM_IMPLIED},            // 0x2B
    {INS_BIT, AM_ABSOLUTE},           // 0x2C
    {INS_AND, AM_ABSOLUTE},           // 0x2D
    {INS_ROL, AM_ABSOLUTE},           // 0x2E
    {INS_INV, AM_IMPLIED},            // 0x2F

    // 0x30 - 0x3F
    {INS_BMI, AM_RELATIVE},           // 0x30
    {INS_AND, AM_INDIRECT_INDEXED_Y}, // 0x31
    {INS_INV, AM_IMPLIED},            // 0x32
    {INS_INV, AM_IMPLIED},            // 0x33
    {INS_INV, AM_IMPLIED},            // 0x34
    {INS_AND, AM_INDEXED_ZEROPAGE_X}, // 0x35
    {INS_ROL, AM_INDEXED_ZEROPAGE_X}, // 0x36
    {INS_INV, AM_IMPLIED},            // 0x37
    {INS_SEC, AM_IMPLIED},            // 0x38
    {INS_AND, AM_INDEXED_ABSOLUTE_Y}, // 0x39
    {INS_INV, AM_IMPLIED},            // 0x3A
    {INS_INV, AM_IMPLIED},            // 0x3B
    {INS_INV, AM_IMPLIED},            // 0x3C
    {INS_AND, AM_INDEXED_ABSOLUTE_X}, // 0x3D
    {INS_ROL, AM_INDEXED_ABSOLUTE_X}, // 0x3E
    {INS_INV, AM_IMPLIED},            // 0x3F

    // 0x40 - 0x4F
    {INS_RTI, AM_IMPLIED},            // 0x40
    {INS_EOR, AM_INDEXED_INDIRECT_X}, // 0x41
    {INS_INV, AM_IMPLIED},            // 0x42
    {INS_INV, AM_IMPLIED},            // 0x43
    {INS_INV, AM_IMPLIED},            // 0x44
    {INS_EOR, AM_ZEROPAGE},           // 0x45
    {INS_LSR, AM_ZEROPAGE},           // 0x46
    {INS_INV, AM_IMPLIED},            // 0x47
    {INS_PHA, AM_IMPLIED},            // 0x48
    {INS_EOR, AM_IMMEDIATE},          // 0x49
    {INS_LSR, AM_ACCUMULATOR},        // 0x4A
    {INS_INV, AM_IMPLIED},            // 0x4B
    {INS_JMP, AM_ABSOLUTE},           // 0x4C
    {INS_EOR, AM_ABSOLUTE},           // 0x4D
    {INS_LSR, AM_ABSOLUTE},           // 0x4E
    {INS_INV, AM_IMPLIED},            // 0x4F

    // 0x50 - 0x5F
    {INS_BVC, AM_RELATIVE},           // 0x50
    {INS_EOR, AM_INDIRECT_INDEXED_Y}, // 0x51
    {INS_INV, AM_IMPLIED},            // 0x52
    {INS_INV, AM_IMPLIED},            // 0x53
    {INS_INV, AM_IMPLIED},            // 0x54
    {INS_EOR, AM_INDEXED_ZEROPAGE_X}, // 0x55
    {INS_LSR, AM_INDEXED_ZEROPAGE_X}, // 0x56
    {INS_INV, AM_IMPLIED},            // 0x57
    {INS_CLI, AM_IMPLIED},            // 0x58
    {INS_EOR, AM_INDEXED_ABSOLUTE_Y}, // 0x59
    {INS_INV, AM_IMPLIED},            // 0x5A
    {INS_INV, AM_IMPLIED},            // 0x5B
    {INS_INV, AM_IMPLIED},            // 0x5C
    {INS_EOR, AM_INDEXED_ABSOLUTE_X}, // 0x5D
    {INS_LSR, AM_INDEXED_ABSOLUTE_X}, // 0x5E
    {INS_INV, AM_IMPLIED},            // 0x5F

    // 0x60 - 0x6F
    {INS_RTS, AM_IMPLIED},            // 0x60
    {INS_ADC, AM_INDEXED_INDIRECT_X}, // 0x61
    {INS_INV, AM_IMPLIED},            // 0x62
    {INS_INV, AM_IMPLIED},            // 0x63
    {INS_INV, AM_IMPLIED},            // 0x64
    {INS_ADC, AM_ZEROPAGE},           // 0x65
    {INS_ROR, AM_ZEROPAGE},           // 0x66
    {INS_INV, AM_IMPLIED},            // 0x67
    {INS_PLA, AM_IMPLIED},            // 0x68
    {INS_ADC, AM_IMMEDIATE},          // 0x69
    {INS_ROR, AM_ACCUMULATOR},        // 0x6A
    {INS_INV, AM_IMPLIED},            // 0x6B
    {INS_JMP, AM_ABSOLUTE_INDIRECT},  // 0x6C
    {INS_ADC, AM_ABSOLUTE},           // 0x6D
    {INS_ROR, AM_ABSOLUTE},           // 0x6E
    {INS_INV, AM_IMPLIED},            // 0x6F

    // 0x70 - 0x7F
    {INS_BVS, AM_RELATIVE},           // 0x70
    {INS_ADC, AM_INDIRECT_INDEXED_Y}, // 0x71
    {INS_INV, AM_IMPLIED},            // 0x72
    {INS_INV, AM_IMPLIED},            // 0x73
    {INS_INV, AM_IMPLIED},            // 0x74
    {INS_ADC, AM_INDEXED_ZEROPAGE_X}, // 0x75
    {INS_ROR, AM_INDEXED_ZEROPAGE_X}, // 0x76
    {INS_INV, AM_IMPLIED},            // 0x77
    {INS_SEI, AM_IMPLIED},            // 0x78
    {INS_ADC, AM_INDEXED_ABSOLUTE_Y}, // 0x79
    {INS_INV, AM_IMPLIED},            // 0x7A
    {INS_INV, AM_IMPLIED},            // 0x7B
    {INS_INV, AM_IMPLIED},            // 0x7C
    {INS_ADC, AM_INDEXED_ABSOLUTE_X}, // 0x7D
    {INS_ROR, AM_INDEXED_ABSOLUTE_X}, // 0x7E
    {INS_INV, AM_IMPLIED},            // 0x7F

    // 0x80 - 0x8F
    {INS_INV, AM_IMPLIED},            // 0x80
    {INS_STA, AM_INDEXED_INDIRECT_X}, // 0x81
    {INS_INV, AM_IMPLIED},            // 0x82
    {INS_INV, AM_IMPLIED},            // 0x83
    {INS_STY, AM_ZEROPAGE},           // 0x84
    {INS_STA, AM_ZEROPAGE},           // 0x85
    {INS_STX, AM_ZEROPAGE},           // 0x86
    {INS_INV, AM_IMPLIED},            // 0x87
    {INS_DEY, AM_IMPLIED},            // 0x88
    {INS_INV, AM_IMPLIED},            // 0x89
    {INS_TXA, AM_IMPLIED},            // 0x8A
    {INS_INV, AM_IMPLIED},            // 0x8B
    {INS_STY, AM_ABSOLUTE},           // 0x8C
    {INS_STA, AM_ABSOLUTE},           // 0x8D
    {INS_STX, AM_ABSOLUTE},           // 0x8E
    {INS_INV, AM_IMPLIED},            // 0x8F

    // 0x90 - 0x9F
    {INS_BCC, AM_RELATIVE},           // 0x90
    {INS_STA, AM_INDIRECT_INDEXED_Y}, // 0x91
    {INS_INV, AM_IMPLIED},            // 0x92
    {INS_INV, AM_IMPLIED},            // 0x93
    {INS_STY, AM_INDEXED_ZEROPAGE_X}, // 0x94
    {INS_STA, AM_INDEXED_ZEROPAGE_X}, // 0x95
    {INS_STX, AM_INDEXED_ZEROPAGE_Y}, // 0x96
    {INS_INV, AM_IMPLIED},            // 0x97
    {INS_TYA, AM_IMPLIED},            // 0x98
    {INS_STA, AM_INDEXED_ABSOLUTE_Y}, // 0x99
    {INS_TXS, AM_IMPLIED},            // 0x9A
    {INS_INV, AM_IMPLIED},            // 0x9B
    {INS_INV, AM_IMPLIED},            // 0x9C
    {INS_STA, AM_INDEXED_ABSOLUTE_X}, // 0x9D
    {INS_INV, AM_IMPLIED},            // 0x9E
    {INS_INV, AM_IMPLIED},            // 0x9F

    // 0xA0 - 0xAF
    {INS_LDY, AM_IMMEDIATE},          // 0xA0
    {INS_LDA, AM_INDEXED_INDIRECT_X}, // 0xA1
    {INS_LDX, AM_IMMEDIATE},          // 0xA2
    {INS_INV, AM_IMPLIED},            // 0xA3
    {INS_LDY, AM_ZEROPAGE},           // 0xA4
    {INS_LDA, AM_ZEROPAGE},           // 0xA5
    {INS_LDX, AM_ZEROPAGE},           // 0xA6
    {INS_INV, AM_IMPLIED},            // 0xA7
    {INS_TAY, AM_IMPLIED},            // 0xA8
    {INS_LDA, AM_IMMEDIATE},          // 0xA9
    {INS_TAX, AM_IMPLIED},            // 0xAA
    {INS_INV, AM_IMPLIED},            // 0xAB
    {INS_LDY, AM_ABSOLUTE},           // 0xAC
    {INS_LDA, AM_ABSOLUTE},           // 0xAD
    {INS_LDX, AM_ABSOLUTE},           // 0xAE
    {INS_INV, AM_IMPLIED},            // 0xAF

    // 0xB0 - 0xBF
    {INS_BCS, AM_RELATIVE},           // 0xB0
    {INS_LDA, AM_INDIRECT_INDEXED_Y}, // 0xB1
    {INS_INV, AM_IMPLIED},            // 0xB2
    {INS_INV, AM_IMPLIED},            // 0xB3
    {INS_LDY, AM_INDEXED_ZEROPAGE_X}, // 0xB4
    {INS_LDA, AM_INDEXED_ZEROPAGE_X}, // 0xB5
    {INS_LDX, AM_INDEXED_ZEROPAGE_Y}, // 0xB6
    {INS_INV, AM_IMPLIED},            // 0xB7
    {INS_CLV, AM_IMPLIED},            // 0xB8
    {INS_LDA, AM_INDEXED_ABSOLUTE_Y}, // 0xB9
    {INS_TSX, AM_IMPLIED},            // 0xBA
    {INS_INV, AM_IMPLIED},            // 0xBB
    {INS_LDY, AM_INDEXED_ABSOLUTE_X}, // 0xBC
    {INS_LDA, AM_INDEXED_ABSOLUTE_X}, // 0xBD
    {INS_LDX, AM_INDEXED_ABSOLUTE_Y}, // 0xBE
    {INS_INV, AM_IMPLIED},            // 0xBF

    // 0xC0 - 0xCF
    {INS_CPY, AM_IMMEDIATE},          // 0xC0
    {INS_CMP, AM_INDEXED_INDIRECT_X}, // 0xC1
    {INS_INV, AM_IMPLIED},            // 0xC2
    {INS_INV, AM_IMPLIED},            // 0xC3
    {INS_CPY, AM_ZEROPAGE},           // 0xC4
    {INS_CMP, AM_ZEROPAGE},           // 0xC5
    {INS_DEC, AM_ZEROPAGE},           // 0xC6
    {INS_INV, AM_IMPLIED},            // 0xC7
    {INS_INY, AM_IMPLIED},            // 0xC8
    {INS_CMP, AM_IMMEDIATE},          // 0xC9
    {INS_DEX, AM_IMPLIED},            // 0xCA
    {INS_INV, AM_IMPLIED},            // 0xCB
    {INS_CPY, AM_ABSOLUTE},           // 0xCC
    {INS_CMP, AM_ABSOLUTE},           // 0xCD
    {INS_DEC, AM_ABSOLUTE},           // 0xCE
    {INS_INV, AM_IMPLIED},            // 0xCF

    // 0xD0 - 0xDF
    {INS_BNE, AM_RELATIVE},           // 0xD0
    {INS_CMP, AM_INDIRECT_INDEXED_Y}, // 0xD1
    {INS_INV, AM_IMPLIED},            // 0xD2
    {INS_INV, AM_IMPLIED},            // 0xD3
    {INS_INV, AM_IMPLIED},            // 0xD4
    {INS_CMP, AM_INDEXED_ZEROPAGE_X}, // 0xD5
    {INS_DEC, AM_INDEXED_ZEROPAGE_X}, // 0xD6
    {INS_INV, AM_IMPLIED},            // 0xD7
    {INS_CLD, AM_IMPLIED},            // 0xD8
    {INS_CMP, AM_INDEXED_ABSOLUTE_Y}, // 0xD9
    {INS_INV, AM_IMPLIED},            // 0xDA
    {INS_INV, AM_IMPLIED},            // 0xDB
    {INS_INV, AM_IMPLIED},            // 0xDC
    {INS_CMP, AM_INDEXED_ABSOLUTE_X}, // 0xDD
    {INS_DEC, AM_INDEXED_ABSOLUTE_X}, // 0xDE
    {INS_INV, AM_IMPLIED},            // 0xDF

    // 0xE0 - 0xEF
    {INS_CPX, AM_IMMEDIATE},          // 0xE0
    {INS_SBC, AM_INDEXED_INDIRECT_X}, // 0xE1
    {INS_INV, AM_IMPLIED},            // 0xE2
    {INS_INV, AM_IMPLIED},            // 0xE3
    {INS_CPX, AM_ZEROPAGE},           // 0xE4
    {INS_SBC, AM_ZEROPAGE},           // 0xE5
    {INS_INC, AM_ZEROPAGE},           // 0xE6
    {INS_INV, AM_IMPLIED},            // 0xE7
    {INS_INX, AM_IMPLIED},            // 0xE8
    {INS_SBC, AM_IMMEDIATE},          // 0xE9
    {INS_NOP, AM_IMPLIED},            // 0xEA
    {INS_INV, AM_IMPLIED},            // 0xEB
    {INS_CPX, AM_ABSOLUTE},           // 0xEC
    {INS_SBC, AM_ABSOLUTE},           // 0xED
    {INS_INC, AM_ABSOLUTE},           // 0xEE
    {INS_INV, AM_IMPLIED},            // 0xEF

    // 0xF0 - 0xFF
    {INS_BEQ, AM_RELATIVE},           // 0xF0
    {INS_SBC, AM_INDIRECT_INDEXED_Y}, // 0xF1
    {INS_INV, AM_IMPLIED},            // 0xF2
    {INS_INV, AM_IMPLIED},            // 0xF3
    {INS_INV, AM_IMPLIED},            // 0xF4
    {INS_SBC, AM_INDEXED_ZEROPAGE_X}, // 0xF5
    {INS_INC, AM_INDEXED_ZEROPAGE_X}, // 0xF6
    {INS_INV, AM_IMPLIED},            // 0xF7
    {INS_SED, AM_IMPLIED},            // 0xF8
    {INS_SBC, AM_INDEXED_ABSOLUTE_Y}, // 0xF9
    {INS_INV, AM_IMPLIED},            // 0xFA
    {INS_INV, AM_IMPLIED},            // 0xFB
    {INS_INV, AM_IMPLIED},            // 0xFC
    {INS_SBC, AM_INDEXED_ABSOLUTE_X}, // 0xFD
    {INS_INC, AM_INDEXED_ABSOLUTE_X}, // 0xFE
    {INS_INV, AM_IMPLIED}             // 0xFF
};

// ----------------------------------------------
// Addressing modes
// These functions resolve the target memory address of an instruction

uint16_t zeropage_addr(uint8_t addr){
	return addr;
}

uint16_t indexed_absolute_addr(uint16_t addr, uint8_t index){
	return addr + index;
}

uint16_t indexed_zeropage_addr(uint8_t addr, uint8_t index){
	return ((addr + index) & 0xff);
}

uint16_t relative_addr(uint16_t addr, int8_t offset){
	return addr + offset;
}

uint16_t indirect_indexed_addr(uint8_t addr, uint8_t index){
	uint16_t ptr = (uint8_t)(addr + index);

	return readMemory(ptr) | (readMemory(1+ptr)<<8);
}

uint16_t indexed_indirect_addr(uint8_t addr, uint8_t index){
	uint16_t ptr;

	ptr = readMemory(addr) | (readMemory(1+addr)<<8);
	return ptr + index;
}

uint16_t absolute_indirect_addr(uint16_t addr){
	uint16_t addr_hi;

	// INTENTIONAL: A hardware bug prevents this addressing mode from crossing page boundaries
	if((addr&0xff) == 0xff){
		addr_hi = addr&0xff00;
	} else{
		addr_hi = 1+addr;
	}

	return readMemory(addr) | (readMemory(addr_hi)<<8);
}

// Print disassembly of instruction at addr;
// Returns address of the next instruction
uint16_t disassemble(uint16_t addr){
	Opcode op;
	uint8_t param8 = readMemory(1+addr);
	uint16_t nextAddr, param16 = readMemory(1+addr) | readMemory(2+addr)<<8;

	memcpy(&op, &opcodes[readMemory(addr)], sizeof(Opcode));
	cprintf("%s", mnemonics[op.instr]);

	nextAddr = addr+instruction_length[op.addr_mode];

	switch(op.addr_mode){
		case AM_ACCUMULATOR:
		case AM_IMPLIED:
		cprintf("        ");
		break;

		case AM_IMMEDIATE:
		cprintf(" #$%02X   ", param8);
		break;

		case AM_ABSOLUTE:
		cprintf(" $%04X  ", param16);
		break;

		case AM_ZEROPAGE:
		cprintf(" $%02X    ", param8);
		break;

		case AM_INDEXED_ZEROPAGE_X:
		cprintf(" $%02X,x  ", param8);
		break;

		case AM_INDEXED_ZEROPAGE_Y:
		cprintf(" $%02X,y  ", param8);
		break;

		case AM_INDEXED_ABSOLUTE_X:
		cprintf(" $%04X,x", param16);
		break;

		case AM_INDEXED_ABSOLUTE_Y:
		cprintf(" $%04X,y", param16);
		break;

		case AM_RELATIVE:
		cprintf(" $%04X  ", relative_addr(nextAddr, (int8_t)param8));
		break;

		case AM_INDEXED_INDIRECT_X:
		cprintf(" ($%02X,x)", param8);
		break;

		case AM_INDIRECT_INDEXED_Y:
		cprintf(" ($%02X),y", param8);
		break;

		case AM_ABSOLUTE_INDIRECT:
		cprintf(" ($%04X)", param16);
		break;
	}

	return nextAddr;
}

// Print current instruction and registers
void debug(){

	cprintf(" %04X: ", mcs6500.pc);
	disassemble(mcs6500.pc);

	cprintf(
		"P:%02X\r\n      A:%02X X:%02X Y:%02X SP:01%02X\r\n",
		mcs6500.p,
		mcs6500.a,
		mcs6500.x,
		mcs6500.y,
        mcs6500.s
	);
}