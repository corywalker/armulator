/*
 * cpsr.h
 *
 *  Created on: Aug 7, 2009
 *      Author: cory
 */

#ifndef CPSR_H_
#define CPSR_H_

#include <stdint.h>

void set_n(uint32_t * CPSR, uint8_t value);
void set_z(uint32_t * CPSR, uint8_t value);
void set_c(uint32_t * CPSR, uint8_t value);
void set_v(uint32_t * CPSR, uint8_t value);
uint8_t get_n(uint32_t * CPSR);
uint8_t get_z(uint32_t * CPSR);
uint8_t get_c(uint32_t * CPSR);
uint8_t get_v(uint32_t * CPSR);

#endif /* CPSR_H_ */
