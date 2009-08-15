/*
 * arm940t.h
 *
 *  Created on: Aug 7, 2009
 *      Author: cory
 */

#ifndef ARM940T_H_
#define ARM940T_H_

#include "util.h"
#include "addrmode1.h"
#include "register.h"
#include "cpsr.h"
#include "types.h"
#include <stdlib.h>

arm940t new_arm940t();
instruction parse_instruction(arm940t * cpu, uint32_t address);
void cycle(arm940t * cpu);
uint8_t condition(arm940t * cpu, uint8_t cond);
uint32_t get_be_word(arm940t * cpu, uint32_t address);
void print_inst(char line[], uint8_t cond, uint8_t sbit);
void delete_arm940t(arm940t * cpu);

#endif /* ARM940T_H_ */
