/*
 * register.h
 *
 *  Created on: Aug 7, 2009
 *      Author: cory
 */

//http://code.google.com/p/gp2xemu/source/browse/trunk/gp2x-emu/src/all-platforms/addrmode1.c?r=70
//http://lpahome.com/geohot/DBS/DBSgui/arm9.h

#ifndef REGISTER_H_
#define REGISTER_H_

#include "types.h"
#include <stdint.h>
#include <stdio.h>

void print_reg(arm940t * cpu, unsigned char reg);
void print_regs(arm940t * cpu);

#endif /* REGISTER_H_ */
