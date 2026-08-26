#include <stdint.h>

#include "6500.h"
#include "alu.h"

uint8_t alu_adc(uint8_t a, uint8_t b){
	uint16_t r;
	uint8_t c = (mcs6500.p&PFLAG_C);

	r = a + b + c;

	mcs6500.p &= (~PFLAG_C & ~PFLAG_V);
	if(r > 0xff)             mcs6500.p |= PFLAG_C;
	if((a^r) & (b^r) & 0x80) mcs6500.p |= PFLAG_V;

	updateNZ(r);

	// Decimal adjust
	// INTENTIONAL: In NMOS 6502, this step is done after processor flags have been
	//  set, resulting in wrong N and V flags in BCD mode
	if(mcs6500.p & PFLAG_D){
		if(((a & 0x0f) + (b & 0x0f) + c) > 0x09)
			r += 0x06;
	    if(r > 0x99) r += 0x60;
	}
	
	return r&0xff;
}

uint8_t alu_sbc(uint8_t a, uint8_t b, uint8_t cmp){
	int16_t r;
	uint8_t c = (mcs6500.p&PFLAG_C) | cmp;

	r = a - b - (1-c);

	mcs6500.p &= ~PFLAG_C;
	if(r <= 0xff) mcs6500.p |= PFLAG_C;

	if(!cmp){
		mcs6500.p &= ~PFLAG_V;
		if((a^b) & (a^r) & 0x80) mcs6500.p |= PFLAG_V;
	}

	updateNZ(r);

	// Decimal adjust
	if(mcs6500.p & PFLAG_D){
		if (((a & 0x0f) + c) <= (b & 0x0f)) r -= 0x06;
    	if (r < 0) r -= 0x60;
	}

	return r&0xff;
}

void updateNZ(uint8_t x){
	mcs6500.p &= (~PFLAG_N & ~PFLAG_Z);
	if(!x)     mcs6500.p |= PFLAG_Z;
	if(x&0x80) mcs6500.p |= PFLAG_N;
}