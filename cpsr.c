#include "cpsr.h"

void set_n(uint32_t * CPSR, uint8_t value)
{
	if(value)
		*CPSR |= 1 << 31;
	else
		*CPSR &= ~(1 << 31);
}

void set_z(uint32_t * CPSR, uint8_t value)
{
	if(value)
		*CPSR |= 1 << 30;
	else
		*CPSR &= ~(1 << 30);
}

void set_c(uint32_t * CPSR, uint8_t value)
{
	if(value)
		*CPSR |= 1 << 29;
	else
		*CPSR &= ~(1 << 29);
}

void set_v(uint32_t * CPSR, uint8_t value)
{
	if(value)
		*CPSR |= 1 << 28;
	else
		*CPSR &= ~(1 << 28);
}

uint8_t get_n(uint32_t * CPSR)
{
	return (*CPSR >> 31) & 1;
}

uint8_t get_z(uint32_t * CPSR)
{
	return (*CPSR >> 30) & 1;
}

uint8_t get_c(uint32_t * CPSR)
{
	return (*CPSR >> 29) & 1;
}

uint8_t get_v(uint32_t * CPSR)
{
	return (*CPSR >> 28) & 1;
}
