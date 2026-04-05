#include <stdint.h>
#include <string.h>

#include "6500.h"
#include "instructions.h"
#include "memory.h"
#include "tracelog.h"

/* ── helpers ──────────────────────────────────────────────────── */

static char hex_char(uint8_t nibble) {
	return nibble < 10 ? '0' + nibble : 'A' + (nibble - 10);
}

static void u8_to_hex(uint8_t v, char *out){
	out[0] = hex_char(v >> 4);
	out[1] = hex_char(v & 0xf);
	out[2] = '\0';
}

static void u16_to_hex(uint16_t v, char *out){
	out[0] = hex_char((v >> 12)&0xf);
	out[1] = hex_char((v >>  8)&0xf);
	out[2] = hex_char((v >>  4)&0xf);
	out[3] = hex_char( v       &0xf);
	out[4] = '\0';
}

// Get disassembly of instruction at addr;
// Returns address of the next instruction
uint16_t disassemble(uint16_t addr, char *out){
	static char buf[12];
	char hbuf[5];   /* scratch: up to 4 hex digits + NUL */
	Opcode   op;
	uint8_t  param8  = readMemory(1 + addr);
	uint16_t param16 = readMemory(1 + addr) | readMemory(2 + addr) << 8;
	uint16_t nextAddr;

	memcpy(&op, &opcodes[readMemory(addr)], sizeof(Opcode));
	nextAddr = addr + instruction_length[op.addr_mode];

	strcpy(buf, mnemonics[op.instr]);   /* always 3 chars */

	switch(op.addr_mode){
		case AM_ACCUMULATOR:
		case AM_IMPLIED:
			strcat(buf, "        ");
			break;
		case AM_IMMEDIATE:
			strcat(buf, " #$");   u8_to_hex(param8,  hbuf);  strcat(buf, hbuf);
			strcat(buf, "   ");
			break;
		case AM_ABSOLUTE:
			strcat(buf, " $");    u16_to_hex(param16, hbuf);  strcat(buf, hbuf);
			strcat(buf, "  ");
			break;
		case AM_ZEROPAGE:
			strcat(buf, " $");    u8_to_hex(param8,  hbuf);  strcat(buf, hbuf);
			strcat(buf, "    ");
			break;
		case AM_INDEXED_ZEROPAGE_X:
			strcat(buf, " $");    u8_to_hex(param8,  hbuf);  strcat(buf, hbuf);
			strcat(buf, ",x  ");
			break;
		case AM_INDEXED_ZEROPAGE_Y:
			strcat(buf, " $");    u8_to_hex(param8,  hbuf);  strcat(buf, hbuf);
			strcat(buf, ",y  ");
			break;
		case AM_INDEXED_ABSOLUTE_X:
			strcat(buf, " $");    u16_to_hex(param16, hbuf);  strcat(buf, hbuf);
			strcat(buf, ",x");
			break;
		case AM_INDEXED_ABSOLUTE_Y:
			strcat(buf, " $");    u16_to_hex(param16, hbuf);  strcat(buf, hbuf);
			strcat(buf, ",y");
			break;
		case AM_RELATIVE:
			strcat(buf, " $");
			u16_to_hex(relative_addr(nextAddr, (int8_t)param8), hbuf);
			strcat(buf, hbuf);
			strcat(buf, "  ");
			break;
		case AM_INDEXED_INDIRECT_X:
			strcat(buf, " ($");   u8_to_hex(param8,  hbuf);  strcat(buf, hbuf);
			strcat(buf, ",x)");
			break;
		case AM_INDIRECT_INDEXED_Y:
			strcat(buf, " ($");   u8_to_hex(param8,  hbuf);  strcat(buf, hbuf);
			strcat(buf, "),y");
			break;
		case AM_ABSOLUTE_INDIRECT:
			strcat(buf, " ($");   u16_to_hex(param16, hbuf);  strcat(buf, hbuf);
			strcat(buf, ")");
			break;
	}

	strcpy(out, buf);
	return nextAddr;
}

char *tracelog(){
	static char buf[64];
	char hbuf[5];
	char disbuf[12];

	buf[0] = '\0';

	strcat(buf, " ");
	u16_to_hex(mcs6500.pc, hbuf);  strcat(buf, hbuf);
	strcat(buf, ": ");

	disassemble(mcs6500.pc, disbuf);
	strcat(buf, disbuf);

	strcat(buf, "P:");
	u8_to_hex(mcs6500.p, hbuf);    strcat(buf, hbuf);
	strcat(buf, "\r\n      A:");
	u8_to_hex(mcs6500.a, hbuf);    strcat(buf, hbuf);
	strcat(buf, " X:");
	u8_to_hex(mcs6500.x, hbuf);    strcat(buf, hbuf);
	strcat(buf, " Y:");
	u8_to_hex(mcs6500.y, hbuf);    strcat(buf, hbuf);
	strcat(buf, " SP:01");
	u8_to_hex(mcs6500.s, hbuf);    strcat(buf, hbuf);
	strcat(buf, "\r\n");

	return buf;
}