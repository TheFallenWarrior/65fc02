#ifndef _65FC_ALU_H
#define _65FC_ALU_H

#include <stdint.h>

uint8_t alu_adc(uint8_t a, uint8_t b);
uint8_t alu_sbc(uint8_t a, uint8_t b, uint8_t cmp);
void updateNZ(uint8_t x);

#endif