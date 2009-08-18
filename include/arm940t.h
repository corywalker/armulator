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
instruction decode_instruction(arm940t * cpu, uint32_t address);
void cycle(arm940t * cpu);
uint8_t condition(arm940t * cpu, uint8_t cond);
uint32_t fetch_instruction(arm940t * cpu, uint32_t address);
void print_operation(char line[], uint8_t cond, uint8_t sbit);
void delete_arm940t(arm940t * cpu);
void run_opcode(arm940t * cpu, uint8_t opcode, uint8_t s, uint8_t rn, uint8_t rd, shifter_result shifted);
void print_instruction(instruction inst);

#endif /* ARM940T_H_ */
