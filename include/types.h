/*
 * types.h
 *
 *  Created on: Aug 8, 2009
 *      Author: cory
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

typedef struct {uint8_t id; uint32_t word; uint8_t cond; uint8_t char0; uint8_t char1;
	uint8_t char2; uint8_t char3; uint16_t short0; uint32_t int0;} instruction;

typedef struct {
	uint8_t NUM_REGS;
	uint32_t R[16];
	uint32_t CPSR;
	uint32_t fetched;
	instruction decoded;
	uint8_t have_fetched;
	uint8_t have_decoded;
	uint8_t * binary;
	int binarysz;
} arm940t;

#endif /* TYPES_H_ */
