#include <stdint.h>

#include "6500.h"
#include "alu.h"

uint16_t bcd_add(uint8_t a, uint8_t b, uint8_t c) {
    int16_t r;

    r = a + b + c;
    if(((a & 0x0f) + (b & 0x0f) + c) > 0x09) r += 0x06;
    if(r > 0x99) r += 0x60;
    return r;
}

int16_t bcd_sub(uint8_t a, uint8_t b, uint8_t c) {
    int16_t r;

    r = a - b - (1 - c);
    if (((a & 0x0f) + c) <= (b & 0x0f)) r -= 0x06;
    if (r < 0) r -= 0x60;
    return r;
}

uint8_t alu_adc(uint8_t a, uint8_t b){
	uint16_t r;

	if(mcs6500.p & PFLAG_D){ // Check if decimal mode
		r = bcd_add(a, b, (mcs6500.p&PFLAG_C));
	} else{
		r = a + b + (mcs6500.p&PFLAG_C);
	}

	mcs6500.p &= (~PFLAG_C & ~PFLAG_V);
	
	if(r > 0xff)             mcs6500.p |= PFLAG_C;
	if((a^r) & (b^r) & 0x80) mcs6500.p |= PFLAG_V;
	
	return r&0xff;
}

uint8_t alu_sbc(uint8_t a, uint8_t b, uint8_t cmp){
	uint16_t r;
	uint8_t c = (mcs6500.p&PFLAG_C) | cmp;

	if(mcs6500.p & PFLAG_D){ // Check if decimal mode
		r = (uint16_t)bcd_sub(a, b, c);
	} else{
		r = a - b - (1 - c);
	}

	mcs6500.p &= ~PFLAG_C;
	if(r <= 0xff) mcs6500.p |= PFLAG_C;

	if(!cmp){
		mcs6500.p &= ~PFLAG_V;
		if((a^b) & (a^r) & 0x80) mcs6500.p |= PFLAG_V;
	}

	return r&0xff;
}

void updateNZ(uint8_t x){
	mcs6500.p &= (~PFLAG_N & ~PFLAG_Z);
	if(!x)     mcs6500.p |= PFLAG_Z;
	if(x&0x80) mcs6500.p |= PFLAG_N;
}