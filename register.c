#include "register.h"

void print_reg(arm940t * cpu, uint8_t reg)
{
	if(reg < cpu->NUM_REGS)
		printf("R%u = %u\n", (int) reg, cpu->R[13]);
	else
		printf("Undefined register;");
}

void print_regs(arm940t * cpu)
{
	uint8_t i;
	for(i = 0; i < cpu->NUM_REGS; i++)
		print_reg(cpu, i);
}
