/*
 * util.h
 *
 *  Created on: Aug 7, 2009
 *      Author: cory
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>

#define ASSERT(cond,string) \
	({ if(!cond) { printf("FATAL: %s", string); exit(1); } })

uint32_t get_bits(uint32_t number, uint8_t start, uint8_t length);
uint32_t rol(uint32_t value, uint32_t shift);
uint32_t ror(uint32_t value, uint32_t shift);
uint32_t asr(int32_t value, uint32_t shift);
uint8_t is_overflow(uint32_t num1, uint32_t num2);
uint8_t is_underflow(uint32_t num1, uint32_t num2);
uint8_t is_signed_overflow(int32_t num1, uint8_t operation, int32_t num2);

#endif /* UTIL_H_ */
