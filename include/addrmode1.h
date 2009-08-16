/*
 * addrmode1.h
 *
 *  Created on: Aug 7, 2009
 *      Author: cory
 */

#ifndef ADDRMODE1_H_
#define ADDRMODE1_H_

#include "util.h"
#include "arm940t.h"
#include "types.h"
//#include "cpsr.h"
#include <stdint.h>


shifter_result addrmode1(arm940t * cpu, uint32_t word);
shifter_result adr1_imm(arm940t * cpu, uint32_t word);
shifter_result adr1_reg(arm940t * cpu, uint32_t word);
shifter_result adr1_lsl_imm(arm940t * cpu, uint32_t word);
shifter_result adr1_lsl_reg(arm940t * cpu, uint32_t word);
shifter_result adr1_lsr_imm(arm940t * cpu, uint32_t word);
shifter_result adr1_lsr_reg(arm940t * cpu, uint32_t word);
shifter_result adr1_asr_imm(arm940t * cpu, uint32_t word);
shifter_result adr1_asr_reg(arm940t * cpu, uint32_t word);
shifter_result adr1_ror_imm(arm940t * cpu, uint32_t word);
shifter_result adr1_ror_reg(arm940t * cpu, uint32_t word);
shifter_result adr1_rrx_reg(arm940t * cpu, uint32_t word);

#endif /* ADDRMODE1_H_ */
