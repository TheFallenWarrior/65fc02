#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "6500.h"
#include "alu.h"
#include "instructions.h"
#include "memory.h"
#include "util.h"

Processor mcs6500;

void pushToStack(uint8_t byte){
	writeMemory(STACK_PTR(), byte);
	--mcs6500.s;
}

uint8_t pullFromStack(){
	uint8_t r;

	++mcs6500.s;
	r = readMemory(STACK_PTR());
	return r;
}

void reset(){
	// Disable interrupts
	mcs6500.p &= ~PFLAG_I;

	// Set PC to Reset routine
	mcs6500.pc = readMemory(1+VEC_RST)<<8 | readMemory(VEC_RST);
}

uint16_t decodeInstruction(DecodedInstruction *ins){
	memcpy(&(ins->operation), &opcodes[mcs6500.ir], sizeof(Opcode));
	ins->param8 = 0;
	ins->param16 = 0;
	ins->pc_mod = 0;

	// Generally speaking,
	//  instructions that read from memory expect the fetched byte at param8; and
	//  instructions that write to memory expect the effective memory address at param16.
	// By extension, read-modify-write instructions use both param8 and param16.
	// This switch satisfies both cases
	switch(ins->operation.addr_mode){
		case AM_IMPLIED:
		break;

		case AM_ACCUMULATOR:
		ins->param8 = mcs6500.a;
		break;

		case AM_IMMEDIATE:
		ins->param8 = readMemory(1+mcs6500.pc);
		break;

		case AM_ABSOLUTE:
		ins->param16 = readMemory(1+mcs6500.pc) | (readMemory(2+mcs6500.pc)<<8);
		ins->param8 = readMemory(ins->param16);
		break;

		case AM_ZEROPAGE:
		ins->param16 = zeropage_addr(readMemory(1+mcs6500.pc));
		ins->param8 = readMemory(ins->param16);
		break;

		case AM_INDEXED_ZEROPAGE_X:
		ins->param16 = indexed_zeropage_addr(
			readMemory(1+mcs6500.pc),
			mcs6500.x
		);
		ins->param8 = readMemory(ins->param16);
		break;

		case AM_INDEXED_ZEROPAGE_Y:
		ins->param16 = indexed_zeropage_addr(
			readMemory(1+mcs6500.pc),
			mcs6500.y
		);
		ins->param8 = readMemory(ins->param16);
		break;

		case AM_INDEXED_ABSOLUTE_X:
		ins->param16 = indexed_absolute_addr(
			readMemory(1+mcs6500.pc) | (readMemory(2+mcs6500.pc)<<8),
			mcs6500.x
		);
		ins->param8 = readMemory(ins->param16);
		break;

		case AM_INDEXED_ABSOLUTE_Y:
		ins->param16 = indexed_absolute_addr(
			readMemory(1+mcs6500.pc)  | (readMemory(2+mcs6500.pc)<<8),
			mcs6500.y
		);
		ins->param8 = readMemory(ins->param16);
		break;

		// Only used by branch instructions
		case AM_RELATIVE:
		ins->param16 = relative_addr(
			mcs6500.pc + instruction_length[ins->operation.addr_mode],
			(int8_t)readMemory(1+mcs6500.pc)
		);
		break;

		case AM_INDEXED_INDIRECT_X:
		ins->param16 = indexed_indirect_addr(
			readMemory(1+mcs6500.pc)  | (readMemory(2+mcs6500.pc)<<8),
			mcs6500.x
		);
		ins->param8 = readMemory(ins->param16);
		break;

		case AM_INDIRECT_INDEXED_Y:
		ins->param16 = indirect_indexed_addr(
			readMemory(1+mcs6500.pc)  | (readMemory(2+mcs6500.pc)<<8),
			mcs6500.y
		);
		ins->param8 = readMemory(ins->param16);
		break;

		// Only used by JMP (indirect)
		case AM_ABSOLUTE_INDIRECT:
		ins->param16 = absolute_indirect_addr(readMemory(1+mcs6500.pc)  | (readMemory(2+mcs6500.pc)<<8));
	}

	return mcs6500.pc + instruction_length[ins->operation.addr_mode];
}

void executeInstruction(DecodedInstruction *ins){
	uint8_t tmp;

	switch(ins->operation.instr){
		// --------------------------------------
		// Invalid and no-op
		case INS_INV:
		case INS_NOP:
		break;

		// --------------------------------------
		// Flag manipulation
		case INS_CLC:
		mcs6500.p &= ~PFLAG_C;
		break;

		case INS_SEC:
		mcs6500.p |= PFLAG_C;
		break;

		case INS_CLI:
		mcs6500.p &= ~PFLAG_I;
		break;

		case INS_SEI:
		mcs6500.p |= PFLAG_I;
		break;

		case INS_CLD:
		mcs6500.p &= ~PFLAG_D;
		break;

		case INS_SED:
		mcs6500.p |= PFLAG_D;
		break;

		case INS_CLV:
		mcs6500.p &= ~PFLAG_V;
		break;

		// --------------------------------------
		// Comparisons
		case INS_CMP:
		tmp = alu_sbc(mcs6500.a, ins->param8, 1);
		updateNZ(tmp);
		break;

		case INS_CPX:
		tmp = alu_sbc(mcs6500.x, ins->param8, 1);
		updateNZ(tmp);
		break;

		case INS_CPY:
		tmp = alu_sbc(mcs6500.y, ins->param8, 1);
		updateNZ(tmp);
		break;

		case INS_BIT:
		mcs6500.p &= (~PFLAG_N & ~PFLAG_V & ~PFLAG_Z);
		mcs6500.p |= ins->param8&0xc0 | ((!(mcs6500.a & ins->param8))<<1);
		break;

		// --------------------------------------
		// Branching
		case INS_BCC:
		if(!(mcs6500.p&PFLAG_C)){
			mcs6500.pc = ins->param16;
			ins->pc_mod = 1;
		} break;

		case INS_BCS:
		if(mcs6500.p&PFLAG_C){
			mcs6500.pc = ins->param16;
			ins->pc_mod = 1;
		} break;

		case INS_BNE:
		if(!(mcs6500.p&PFLAG_Z)){
			mcs6500.pc = ins->param16;
			ins->pc_mod = 1;
		} break;

		case INS_BEQ:
		if(mcs6500.p&PFLAG_Z){
			mcs6500.pc = ins->param16;
			ins->pc_mod = 1;
		} break;

		case INS_BVC:
		if(!(mcs6500.p&PFLAG_V)){
			mcs6500.pc = ins->param16;
			ins->pc_mod = 1;
		} break;

		case INS_BVS:
		if(mcs6500.p&PFLAG_V){
			mcs6500.pc = ins->param16;
			ins->pc_mod = 1;
		} break;

		case INS_BPL:
		if(!(mcs6500.p&PFLAG_N)){
			mcs6500.pc = ins->param16;
			ins->pc_mod = 1;
		} break;

		case INS_BMI:
		if(mcs6500.p&PFLAG_N){
			mcs6500.pc = ins->param16;
			ins->pc_mod = 1;
		} break;

		// --------------------------------------
		// Inconditional jumps
		case INS_JMP:
		mcs6500.pc = ins->param16;
		ins->pc_mod = 1;
		break;

		case INS_JSR:
		mcs6500.pc += instruction_length[AM_ABSOLUTE]-1;
		pushToStack(HI_BYTE(mcs6500.pc));
		pushToStack(LO_BYTE(mcs6500.pc));
		mcs6500.pc = ins->param16;
		ins->pc_mod = 1;
		break;

		case INS_RTS:
		tmp = pullFromStack();
		mcs6500.pc = pullFromStack()<<8 | tmp;
		++mcs6500.pc;
		ins->pc_mod = 1;
		break;

		case INS_BRK:
		mcs6500.pc += instruction_length[AM_ABSOLUTE]-1;
		pushToStack(HI_BYTE(mcs6500.pc));
		pushToStack(LO_BYTE(mcs6500.pc));
		mcs6500.pc = readMemory(1+VEC_IRQ)<<8 | readMemory(VEC_IRQ);
		pushToStack(mcs6500.p);
		mcs6500.p |= PFLAG_B | PFLAG_I;
		ins->pc_mod = 1;
		break;

		case INS_RTI:
		mcs6500.p = pullFromStack();
		tmp = pullFromStack();
		mcs6500.pc = pullFromStack()<<8 | tmp;
		++mcs6500.pc;
		ins->pc_mod = 1;
		break;

		// --------------------------------------
		// Register manipulation
		case INS_TAX:
		mcs6500.x = mcs6500.a;
		updateNZ(mcs6500.a);
		break;

		case INS_TAY:
		mcs6500.y = mcs6500.a;
		updateNZ(mcs6500.a);
		break;

		case INS_TXA:
		mcs6500.a = mcs6500.x;
		updateNZ(mcs6500.x);
		break;

		case INS_TYA:
		mcs6500.a = mcs6500.y;
		updateNZ(mcs6500.y);
		break;

		case INS_TSX:
		mcs6500.x = mcs6500.s;
		updateNZ(mcs6500.s);
		break;

		case INS_TXS:
		mcs6500.s = mcs6500.x;
		break;

		case INS_PHA:
		pushToStack(mcs6500.a);
		break;

		case INS_PLA:
		mcs6500.a = pullFromStack();
		updateNZ(mcs6500.a);
		break;

		case INS_PHP:
		pushToStack(mcs6500.p);
		break;

		case INS_PLP:
		mcs6500.p = pullFromStack();
		break;

		// --------------------------------------
		// Memory manipulation
		case INS_LDA:
		mcs6500.a = ins->param8;
		updateNZ(ins->param8);
		break;

		case INS_LDX:
		mcs6500.x = ins->param8;
		updateNZ(ins->param8);
		break;

		case INS_LDY:
		mcs6500.y = ins->param8;
		updateNZ(ins->param8);
		break;

		case INS_STA:
		writeMemory(ins->param16, mcs6500.a);
		break;

		case INS_STX:
		writeMemory(ins->param16, mcs6500.x);
		break;

		case INS_STY:
		writeMemory(ins->param16, mcs6500.y);
		break;

		// --------------------------------------
		// Logic-arithmetic
		case INS_ADC:
		mcs6500.a = alu_adc(mcs6500.a, ins->param8);
		updateNZ(mcs6500.a);
		break;

		case INS_SBC:
		mcs6500.a = alu_sbc(mcs6500.a, ins->param8, 0);
		updateNZ(mcs6500.a);
		break;

		case INS_INC:
		++ins->param8;
		writeMemory(ins->param16, ins->param8);
		updateNZ(ins->param8);
		break;

		case INS_INX:
		++mcs6500.x;
		updateNZ(mcs6500.x);
		break;

		case INS_INY:
		++mcs6500.y;
		updateNZ(mcs6500.y);
		break;

		case INS_DEC:
		--ins->param8;
		writeMemory(ins->param16, ins->param8);
		updateNZ(ins->param8);
		break;

		case INS_DEX:
		--mcs6500.x;
		updateNZ(mcs6500.x);
		break;

		case INS_DEY:
		--mcs6500.y;
		updateNZ(mcs6500.y);
		break;

		case INS_AND:
		mcs6500.a = mcs6500.a & ins->param8;
		updateNZ(mcs6500.a);
		break;

		case INS_ORA:
		mcs6500.a = mcs6500.a | ins->param8;
		updateNZ(mcs6500.a);
		break;

		case INS_EOR:
		mcs6500.a = mcs6500.a ^ ins->param8;
		updateNZ(mcs6500.a);
		break;

		case INS_ASL:
		mcs6500.p |= !!(ins->param8&0x80);
		ins->param8 <<= 1;
		// If not in accumulator mode, use Read-Modify-Write
		if(ins->operation.addr_mode != AM_ACCUMULATOR)
			writeMemory(ins->param16, ins->param8);
		else mcs6500.a = ins->param8;
		updateNZ(ins->param8);
		break;

		case INS_LSR:
		mcs6500.p |= !!(ins->param8&0x01);
		ins->param8 >>= 1;
		if(ins->operation.addr_mode != AM_ACCUMULATOR)
			writeMemory(ins->param16, ins->param8);
		else mcs6500.a = ins->param8;
		updateNZ(ins->param8);
		break;

		case INS_ROL:
		tmp = !!(ins->param8&0x80);
		ins->param8 <<= 1 | (mcs6500.p&PFLAG_C);
		mcs6500.p &= ~PFLAG_C;
		mcs6500.p |= tmp;

		if(ins->operation.addr_mode != AM_ACCUMULATOR)
			writeMemory(ins->param16, ins->param8);
		else mcs6500.a = ins->param8;
		updateNZ(ins->param8);
		break;

		case INS_ROR:
		tmp = ins->param8&0x01;
		ins->param8 >>= 1 | (mcs6500.p&PFLAG_C) << 7;
		mcs6500.p &= ~PFLAG_C;
		mcs6500.p |= tmp;

		if(ins->operation.addr_mode != AM_ACCUMULATOR)
			writeMemory(ins->param16, ins->param8);
		else mcs6500.a = ins->param8;
		updateNZ(ins->param8);
		break;
	}
}

// One Fetch-Decode-Execute cycle
void fetchDecodeExecute(uint8_t debug_enable){
	uint16_t nextAddr;
	DecodedInstruction ins;

	mcs6500.ir = readMemory(mcs6500.pc);
	nextAddr = decodeInstruction(&ins);

	// Trace log the instruction BEFORE executing it
	if(debug_enable) debug();

	executeInstruction(&ins);

	if(!ins.pc_mod){
		mcs6500.pc = nextAddr;
	}
}