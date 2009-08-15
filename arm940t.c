#include "arm940t.h"

instruction parse_instruction(arm940t * cpu, uint32_t word)
{
	instruction inst;
	inst.word = word;
	inst.cond = word >> 28;

	if((get_bits(word, 26, 2) == 0) && (get_bits(word, 21, 4) == 13) &&
		!(!get_bits(word, 25, 1) && get_bits(word, 4, 1) && get_bits(word, 7, 1)))
	{
		inst.id = 1; //MOV
		inst.char0 = get_bits(word, 20, 1); //S
		inst.char1 = get_bits(word, 16, 4); //SBZ
		inst.char2 = get_bits(word, 12, 4); //Rd
		inst.short0 = get_bits(word, 0, 12); //shifter_operand
		print_inst("mov", inst.cond, inst.char0);
		printf(" R%i, %u\n", inst.char2, inst.short0);
	}
	else if((get_bits(word, 26, 2) == 0) && (get_bits(word, 21, 4) == 4))
	{
		inst.id = 2; //ADD
		inst.char0 = get_bits(word, 20, 1); //S
		inst.char1 = get_bits(word, 16, 4); //Rn
		inst.char2 = get_bits(word, 12, 4); //Rd
		inst.short0 = get_bits(word, 0, 12); //shifter_operand
		print_inst("add", inst.cond, inst.char0);
		printf(" R%i, R%i, %u\n", inst.char2, inst.char1, inst.short0);
	}
	else if((get_bits(word, 26, 2) == 0) && (get_bits(word, 21, 4) == 10))
	{
		inst.id = 3; //CMP
		inst.char0 = get_bits(word, 16, 4); //Rn
		inst.char1 = get_bits(word, 12, 4); //SBZ
		inst.short0 = get_bits(word, 0, 12); //shifter_operand
		print_inst("cmp", inst.cond, 0);
		printf(" R%i, %u\n", inst.char0, inst.short0);
	}
	else if(get_bits(word, 25, 3) == 5)
	{
		inst.id = 4; //B
		inst.char0 = get_bits(word, 24, 1); //L
		inst.int0 = get_bits(word, 0, 24); //target_address
		print_inst("b", inst.cond, inst.char0);
		printf(" %u\n", inst.int0);
	}
	else
	{
		inst.id = 0; //undefined
		printf("other\n");
	}
	return inst;
}

void cycle(arm940t * cpu)
{
	if(cpu->have_decoded)
	{
		printf("   ");
		if(condition(cpu, cpu->decoded.cond))
		{
			uint32_t oldpc = cpu->R[15];
			switch (cpu->decoded.id)
			{
				case 0:
					printf("Undefined\n");
					break;
				case 1:
					cpu->R[cpu->decoded.char2] = addrmode1(cpu, cpu->decoded.word).shifter_operand;
					printf("R%u = %u\n", cpu->decoded.char2, cpu->R[cpu->decoded.char2]);
					break;
				case 2:
					cpu->R[cpu->decoded.char2] = cpu->R[cpu->decoded.char1] + addrmode1(cpu, cpu->decoded.word).shifter_operand;
					printf("R%u incremented to %u\n", cpu->decoded.char2, cpu->R[cpu->decoded.char2]);
					break;
				case 3:
					set_n(&cpu->CPSR, (cpu->R[cpu->decoded.char0] - addrmode1(cpu, cpu->decoded.word).shifter_operand) & (1 << 31));
					set_z(&cpu->CPSR, cpu->R[cpu->decoded.char0] - addrmode1(cpu, cpu->decoded.word).shifter_operand == 0);
					set_c(&cpu->CPSR, !is_underflow(cpu->R[cpu->decoded.char0], addrmode1(cpu, cpu->decoded.word).shifter_operand));
					set_v(&cpu->CPSR, is_signed_overflow(cpu->R[cpu->decoded.char0], 1, addrmode1(cpu, cpu->decoded.word).shifter_operand));
					printf("n=%u z=%u c=%u v=%u\n", get_n(&cpu->CPSR), get_z(&cpu->CPSR), get_c(&cpu->CPSR), get_v(&cpu->CPSR));
					break;
				case 4:
					if(get_bits(cpu->decoded.int0, 23, 1))
						cpu->decoded.int0 += 1056964608;
					cpu->decoded.int0 <<= 2;
					cpu->R[15] += cpu->decoded.int0;
					printf("jumping to %u\n", cpu->R[15]);
					break;
				default:
					printf("Behavior not defined\n");
					break;
			}
			if(oldpc != cpu->R[15])
			{
				cpu->have_decoded = 0;
				cpu->have_fetched = 0;
			}
		}
		else
			printf("Condition not met\n");
	}
	if(cpu->have_fetched)
	{
		cpu->decoded = parse_instruction(cpu, cpu->fetched);
		cpu->have_decoded = 1;
	}
	if(cpu->R[15] < cpu->binarysz)
	{
		cpu->fetched = get_be_word(cpu, cpu->R[15]); //wtf! find out what this is! -1
		cpu->have_fetched = 1;
	}
	else
		cpu->have_fetched = 0;
	cpu->R[15] += 4;
}

void print_inst(char line[], uint8_t cond, uint8_t sbit)
{
	printf("%s", line);
	const char arm_cond[16][3]={"eq","ne","hs","lo","mi","pl","vs","vc","hi",
			"ls","ge","lt","gt","le","",""};
	printf("%s", arm_cond[cond]);
	if(sbit)
		printf("s");
}
uint8_t condition(arm940t * cpu, uint8_t cond)
{
	switch(cond)
	{
	case 0:
		return get_z(&cpu->CPSR);
	case 1:
		return !get_z(&cpu->CPSR);
	case 2:
		return get_c(&cpu->CPSR);
	case 3:
		return !get_c(&cpu->CPSR);
	case 4:
		return get_n(&cpu->CPSR);
	case 5:
		return !get_n(&cpu->CPSR);
	case 6:
		return get_v(&cpu->CPSR);
	case 7:
		return !get_v(&cpu->CPSR);
	case 8:
		return get_c(&cpu->CPSR) && !get_v(&cpu->CPSR);
	case 9:
		return !get_c(&cpu->CPSR) || get_v(&cpu->CPSR);
	case 10:
		return get_n(&cpu->CPSR) == get_v(&cpu->CPSR);
	case 11:
		return get_n(&cpu->CPSR) != get_v(&cpu->CPSR);
	case 12:
		return (!get_z(&cpu->CPSR)) && (get_n(&cpu->CPSR) == get_v(&cpu->CPSR));
	case 13:
		return (get_z(&cpu->CPSR)) && (get_n(&cpu->CPSR) != get_v(&cpu->CPSR));
	case 14:
		return 1;
	case 15:// unconditional expression
		return 1;
	default:
		return 0;
	}
}

uint32_t get_be_word(arm940t * cpu, uint32_t address)
{
	uint32_t word = 0;
	if(address <= cpu->binarysz-4)
	{
		word += cpu->binary[address];
		word += cpu->binary[address+1] << 8;
		word += cpu->binary[address+2] << 16;
		word += cpu->binary[address+3] << 24;
	}
	else
		printf("Address out of range");
	return word;
}

arm940t new_arm940t()
{
	arm940t ret;
	ret.NUM_REGS = 16;
	// clear regs;
	int i;
	for(i = 0; i < ret.NUM_REGS; i++)
		ret.R[i] = 0;
	ret.have_fetched = 0;
	ret.have_decoded = 0;
	return ret;
}

void delete_arm940t(arm940t * cpu)
{
	free(cpu->binary);
}
